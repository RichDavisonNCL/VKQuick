/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Buffer.h"
#include "MemoryManager.h"
#include "Mesh.h"

namespace VKQuick {
	class MemoryManager;

	class MeshBuilder	{
	public:
		MeshBuilder(vk::Device device, MemoryManager& memManager);
		~MeshBuilder() = default;

		MeshBuilder& WithVertexCount(uint32_t count);
		MeshBuilder& WithIndexCount(uint32_t count, vk::IndexType type);

		MeshBuilder& WithBufferUsageFlags(vk::BufferUsageFlags flags);

		MeshBuilder& WithHostVisibleBuffers();

		MeshBuilder& WithVertexAttribute(uint32_t index, vk::Format format, size_t stride, vk::VertexInputRate rate = vk::VertexInputRate::eVertex);

		UniqueMesh Build();

	private:
		VKQuick::Mesh*			m_mesh;

		vk::Device				m_sourceDevice;
		MemoryManager*			m_memManager;

		vk::CommandBuffer		m_cmdBuffer;

		vk::BufferUsageFlags	m_usageFlags;

		uint32_t				m_vertCount				= 0;
		uint32_t				m_indexCount			= 0;
	};
}