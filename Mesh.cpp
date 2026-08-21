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

bool Mesh::GeAttributeData(uint32_t index, AttributeData& data) const {
	for (int i = 0; i < m_attributeDescriptions.size(); ++i) {
		const vk::VertexInputAttributeDescription& description = m_attributeDescriptions[i];
		if (description.location == index) {
			data.offset		= m_usedOffsets[i];
			data.size		= m_reservedAttributeSizes[i];
			data.format		= description.format;
			data.stride		= m_attributeBindings[i].stride;
			return true;
		}
	}
	return false;
}

bool Mesh::GetIndexData(IndexData& data) const {
	if (m_reservedIndexSize == 0) {
		return false;
	}
	data.offset	= m_indexDataOffset;
	data.size	= m_reservedIndexSize;
	data.type	= m_indexType;
	return true;
}

bool Mesh::IsHostVisibleBuffer() const {
	return false;
}

const std::vector<MeshRange>& Mesh::GetRanges() const {
	return m_meshRanges;
}

size_t Mesh::GetVertexCount() const {
	return m_vertCount;
}

size_t Mesh::GetIndexCount()  const {
	return m_indexCount;
}

bool	Mesh::GetAttributeIndex(VKQuick::AttributeType type, size_t& index) const {
	for (int i = 0; i < m_attributeTypes.size(); ++i) {
		if (m_attributeTypes[i] == type) {
			index = m_attributeDescriptions[i].location;
			return true;
		}
	}
	return false;
}

const VKQuick::Buffer& Mesh::GetBuffer() const {
	return m_gpuBuffer;
}

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
		copyRegion.size = m_gpuBuffer.GetSize();

		cmdBuffer.copyBuffer(m_stagingBuffer, m_gpuBuffer, copyRegion);

		m_memManager->DiscardBuffer(m_stagingBuffer);
	}
}

void Mesh::BindToCommandBuffer(vk::CommandBuffer  buffer) const {
	buffer.bindVertexBuffers(0, m_usedBuffers.size(), &m_usedBuffers[0], &m_usedOffsets[0]);

	if (m_indexType != vk::IndexType::eNoneKHR) {
		buffer.bindIndexBuffer(m_gpuBuffer, m_indexDataOffset, m_indexType);
	}
}

void Mesh::DrawLayer(uint32_t layer, vk::CommandBuffer  to, uint32_t instanceCount) {
	if (layer >= m_meshRanges.size()) {
		return;
	}

	MeshRange mr = m_meshRanges[layer];
	if (GetIndexCount() > 0) {
		to.drawIndexed(mr.count, instanceCount, mr.start, mr.base, 0);
	}
	else {
		to.draw(mr.count, instanceCount, mr.start, 0);
	}
}

void Mesh::DrawBuffers(vk::CommandBuffer  to, uint32_t instanceCount) {
	if (GetIndexCount() > 0) {
		to.drawIndexed(GetIndexCount(), instanceCount, 0, 0, 0);
	}
	else {
		to.draw(GetVertexCount(), instanceCount, 0, 0);
	}
}

void Mesh::Draw(vk::CommandBuffer  to, uint32_t instanceCount) {
	if (m_meshRanges.empty()) {
		DrawBuffers(to, instanceCount);
		return;
	}
	if (GetIndexCount() > 0) {
		for (MeshRange mr : m_meshRanges) {
			to.drawIndexed(mr.count, instanceCount, mr.start, mr.base, 0);
		}
	}
	else {
		for (MeshRange mr : m_meshRanges) {
			to.draw(mr.count, instanceCount, mr.start, 0);
		}
	}
}