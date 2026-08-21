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

	struct MeshRange {
		int start	= 0;
		int count	= 0;
		int base	= 0;
	};

	struct AttributeData {
		uint32_t	offset;
		size_t		size;
		vk::Format	format;
		size_t		stride;
	};

	struct IndexData {
		uint32_t		offset;
		size_t			size;
		vk::IndexType	type;
	};

	enum class AttributeType {
		Position,
		Colour,
		TexCoord,
		TexCoord2,
		Normals,
		Tangents,
		UserData
	};

	class Mesh {
		friend class MeshBuilder;
	public:

		~Mesh();

		const vk::PipelineVertexInputStateCreateInfo& GetVertexInputState() const {
			return m_vertexInputState;
		}		
		
		void BindToCommandBuffer(vk::CommandBuffer  buffer) const;
		void DrawBuffers(vk::CommandBuffer  to, uint32_t instanceCount = 1);
		void DrawLayer(uint32_t layer, vk::CommandBuffer  to, uint32_t instanceCount = 1);
		void Draw(vk::CommandBuffer  to, uint32_t instanceCount = 1);

		bool GeAttributeData(uint32_t index, AttributeData& data) const;
		bool GetIndexData(IndexData& data) const;

		bool IsHostVisibleBuffer() const;

		void*	MapData();
		void	UnmapData(vk::CommandBuffer  cmdBuffer);

		const std::vector<MeshRange>& GetRanges() const;

		size_t GetVertexCount() const;
		size_t GetIndexCount()  const;

		bool	GetAttributeIndex(VKQuick::AttributeType type, size_t& index) const;

		//size_t GetPositionAttributeIndex() const;

		const VKQuick::Buffer& GetBuffer() const;

	private:		
		Mesh();
		vk::PipelineVertexInputStateCreateInfo				m_vertexInputState;

		vk::IndexType	m_indexType	= vk::IndexType::eNoneKHR;

		//Eventually this will support a multi-buffer solution...
		VKQuick::Buffer	m_gpuBuffer;
		VKQuick::Buffer m_stagingBuffer;

		MemoryManager* m_memManager = nullptr;

		size_t		m_allocationSize	= 0;
		size_t		m_vertexDataOffset	= 0;
		size_t		m_indexDataOffset	= 0;
		size_t		m_vertCount			= 0;
		size_t		m_indexCount		= 0;
		size_t		m_positionAttribIndex = 0;

		bool		m_hostVisibleBuffers = false;

		std::vector<vk::Buffer>					m_usedBuffers;
		std::vector<vk::DeviceSize>				m_usedOffsets;
		
		std::vector<vk::VertexInputAttributeDescription>	m_attributeDescriptions;
		std::vector<vk::VertexInputBindingDescription>		m_attributeBindings;
		std::vector<VKQuick::AttributeType>					m_attributeTypes;

		std::vector<MeshRange>		m_meshRanges;
		
		std::vector<size_t>	m_reservedAttributeSizes;
		size_t m_reservedIndexSize = 0;
	};

	using UniqueMesh = std::unique_ptr<Mesh>;
	using SharedMesh = std::shared_ptr<Mesh>;
}