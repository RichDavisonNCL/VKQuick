/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "Mesh.h"
#include "Utils.h"
#include "MemoryManager.h"

using namespace VKQuick;

Mesh::Mesh() {

}

Mesh::~Mesh() {

}

void Mesh::BindToCommandBuffer(vk::CommandBuffer  buffer) const {
	buffer.bindVertexBuffers(0, m_usedBuffers.size(), &m_usedBuffers[0], &m_usedOffsets[0]);

	if (m_indexType != vk::IndexType::eNoneKHR) {
		buffer.bindIndexBuffer(m_gpuBuffer.buffer, m_indexDataOffset, m_indexType);
	}
}

bool Mesh::GetOffsetForAttribute(uint32_t index, uint32_t& offset, size_t& size) {
	for (int i = 0; i < m_attributeDescriptions.size(); ++i) {
		vk::VertexInputAttributeDescription& description = m_attributeDescriptions[i];
		if (description.location == index) {
			offset	= m_usedOffsets[i];
			size	= m_reservedAttributeSizes[i];

			return true;
		}
	}
	return false;
}

bool Mesh::GetOffsetForIndices(uint32_t& offset, size_t& size) {
	if (m_reservedIndexSize == 0) {
		return false;
	}
	offset	= m_indexDataOffset;
	size	= m_reservedIndexSize;
	return true;
}

bool Mesh::IsHostVisibleBuffer() const {
	return false;
}

//void Mesh::WriteAttribute(int i, void* data, size_t dataSize, size_t offset) {
//
//}
//
//void Mesh::WriteIndices(void* data, size_t dataSize, size_t offset) {
//
//}

void* Mesh::MapData() {
	if (IsHostVisibleBuffer()) {
		return m_gpuBuffer.Map();
	}
	else {
		//Need a staging buffer!
		m_stagingBuffer = m_memManager->CreateStagingBuffer(m_allocationSize);

		return m_stagingBuffer.Map();
	}
}

void  Mesh::UnmapData(vk::CommandBuffer  cmdBuffer) {
	if (IsHostVisibleBuffer()) {
		return m_gpuBuffer.Unmap();
	}
	else {
		m_stagingBuffer.Unmap();
		//Copy it over now!

		vk::BufferCopy copyRegion;
		copyRegion.size = m_gpuBuffer.size;

		cmdBuffer.copyBuffer(m_stagingBuffer.buffer, m_gpuBuffer.buffer, copyRegion);

		m_memManager->DiscardBuffer(m_stagingBuffer);
	}
}