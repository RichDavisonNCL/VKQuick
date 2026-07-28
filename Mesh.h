/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Buffer.h"

namespace VKQuick {
	class MemoryManager;

	class Mesh {
		friend class MeshBuilder;
	public:
		Mesh();

		Mesh(std::vector<vk::VertexInputAttributeDescription> attributes, std::vector<vk::VertexInputBindingDescription> bindings);
		~Mesh();

		Mesh(Mesh& other) {
		}

		const vk::PipelineVertexInputStateCreateInfo& GetVertexInputState() const {
			return m_vertexInputState;
		}		
		
		void BindToCommandBuffer(vk::CommandBuffer  buffer) const;

		bool GetOffsetForAttribute(uint32_t index, uint32_t& offset, size_t& size);
		bool GetOffsetForIndices(uint32_t& offset, size_t& size);

		bool IsHostVisibleBuffer() const;

		//void WriteAttribute(int i, void* data, size_t dataSize, size_t offset = 0);
		//void WriteIndices(void* data, size_t dataSize, size_t offset = 0);

		void*	MapData();
		void	UnmapData(vk::CommandBuffer  cmdBuffer);

	private:
		vk::PipelineVertexInputStateCreateInfo				m_vertexInputState;

		vk::IndexType	m_indexType	= vk::IndexType::eNoneKHR;

		VKQuick::Buffer	m_gpuBuffer;
		VKQuick::Buffer m_stagingBuffer;

		MemoryManager* m_memManager = nullptr;

		size_t		m_allocationSize	= 0;
		size_t		m_vertexDataOffset	= 0;
		size_t		m_indexDataOffset	= 0;

		bool		m_hostVisibleBuffers = false;

		std::vector<vk::Buffer>					m_usedBuffers;
		std::vector<vk::DeviceSize>				m_usedOffsets;
		
		std::vector<vk::VertexInputAttributeDescription>	m_attributeDescriptions;
		std::vector<vk::VertexInputBindingDescription>		m_attributeBindings;

		std::vector<size_t>	m_reservedAttributeSizes;
		size_t m_reservedIndexSize = 0;
	};

	using UniqueMesh = std::unique_ptr<Mesh>;
	using SharedMesh = std::shared_ptr<Mesh>;
}