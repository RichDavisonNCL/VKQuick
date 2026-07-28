/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "MeshBuilder.h"
#include "Mesh.h"
#include "Utils.h"

using namespace VKQuick;

size_t GetIndexSize(vk::IndexType type) {
    switch (type) {
        case vk::IndexType ::eUint16:   return sizeof(uint16_t);
        case vk::IndexType::eUint32:    return sizeof(uint32_t);
        case vk::IndexType::eUint8:     return sizeof(uint8_t);
    }
    return 0;
}

MeshBuilder::MeshBuilder(vk::Device inDevice, MemoryManager& memManager) {
    m_sourceDevice      = inDevice;
    m_memManager        = &memManager;

    m_mesh              = new Mesh();
}

MeshBuilder& MeshBuilder::WithVertexCount(uint32_t count) {
    m_mesh->m_vertCount = count;
    return *this;
}

MeshBuilder& MeshBuilder::WithIndexCount(uint32_t count, vk::IndexType type) {
    m_mesh->m_indexCount    = count;
    m_mesh->m_indexType     = type;
    return *this;
}

MeshBuilder& MeshBuilder::WithBufferUsageFlags(vk::BufferUsageFlags flags) {
    m_usageFlags = flags;
    return *this;
}

MeshBuilder& MeshBuilder::WithMeshRange(int start, int vertCount, int baseVertex) {
    MeshRange r;
    r.start = start;
    r.count = vertCount;
    r.base  = baseVertex;

    m_mesh->m_meshRanges.push_back(r);

    return *this;
}

MeshBuilder& MeshBuilder::WithPositionAttributeIndex(uint32_t index) {
    m_mesh->m_positionAttribIndex = index;
    return *this;
}

MeshBuilder& MeshBuilder::WithHostVisibleBuffers() {
    m_mesh->m_hostVisibleBuffers = true;
    return *this;
}

MeshBuilder& MeshBuilder::WithVertexAttribute(uint32_t index, vk::Format format, size_t stride, vk::VertexInputRate inputRate) {
    vk::VertexInputAttributeDescription attribute;
    vk::VertexInputBindingDescription binding;

    attribute.binding   = m_mesh->m_attributeDescriptions.size();
    attribute.format    = format;
    attribute.location  = index;
    attribute.offset    = 0;

    binding.binding     = m_mesh->m_attributeDescriptions.size();
    binding.inputRate   = inputRate;
    binding.stride      = stride;

    m_mesh->m_attributeDescriptions.push_back(attribute);
    m_mesh->m_attributeBindings.push_back(binding);

    return *this;
}

UniqueMesh MeshBuilder::Build() {
    //NOW we go through the attribute descriptions to work out the buffer size and offsets...

    size_t bufferSize = 0;

    std::vector<size_t> attribAllocations;

    for (int i = 0; i < m_mesh->m_attributeDescriptions.size(); ++i) {
        vk::VertexInputAttributeDescription&    attribute    = m_mesh->m_attributeDescriptions[i];
        vk::VertexInputBindingDescription&      binding      = m_mesh->m_attributeBindings[i];

        size_t attributeSize = m_mesh->m_vertCount * binding.stride;

        attribAllocations.push_back(attributeSize);

        attribute.offset = bufferSize;
        bufferSize += attributeSize;
    }

    if (m_mesh->m_indexCount > 0) {
        m_mesh->m_indexDataOffset = bufferSize;

        bufferSize += m_mesh->m_indexCount * GetIndexSize(m_mesh->m_indexType);
    }

    vk::MemoryPropertyFlags	bufferFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

    if (m_mesh->m_hostVisibleBuffers) {
        bufferFlags |= vk::MemoryPropertyFlagBits::eHostVisible;
    }

    m_mesh->m_gpuBuffer = m_memManager->CreateBuffer(
        {
        	.size = bufferSize,
        	.usage =    vk::BufferUsageFlagBits::eIndexBuffer  |
        				vk::BufferUsageFlagBits::eVertexBuffer |
        				vk::BufferUsageFlagBits::eTransferDst  |
                        m_usageFlags
        },
        bufferFlags,
        "Vertex / Index Buffer"
    );

    for (int i = 0; i < m_mesh->m_attributeDescriptions.size(); ++i) {
        m_mesh->m_usedBuffers.push_back(m_mesh->m_gpuBuffer.buffer);

        m_mesh->m_usedOffsets.push_back(m_mesh->m_attributeDescriptions[i].offset); //TODO ???
        m_mesh->m_attributeDescriptions[i].offset = 0;
    }

    m_mesh->m_memManager = m_memManager;

    m_mesh->m_allocationSize    = bufferSize;
    m_mesh->m_vertexDataOffset  = 0;

    m_mesh->m_reservedAttributeSizes    = attribAllocations;
    m_mesh->m_reservedIndexSize         = m_mesh->m_indexCount * GetIndexSize(m_mesh->m_indexType);

    m_mesh->m_vertexInputState = vk::PipelineVertexInputStateCreateInfo(
		{
			.flags = {},
			.vertexBindingDescriptionCount      = (uint32_t)m_mesh->m_attributeBindings.size(),
			.pVertexBindingDescriptions         = &m_mesh->m_attributeBindings[0],
			.vertexAttributeDescriptionCount    = (uint32_t)m_mesh->m_attributeDescriptions.size(),
			.pVertexAttributeDescriptions       = &m_mesh->m_attributeDescriptions[0]
		}
	);

    return UniqueMesh(m_mesh);
}