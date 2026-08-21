///******************************************************************************
//This file is part of the VKQuick library
//
//Author:Rich Davison
//Contact:richgdavison@gmail.com
//License: MIT (see LICENSE file at the top of the source tree)
//*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Instance.h"
#include "Buffer.h"
#include "Mesh.h"
#include "BVHBuilder.h"

namespace VKQuick {

	struct BVHInput {
		VKQuick::Mesh* mesh;
		vk::TransformMatrixKHR transform;
		uint32_t	hitID = 0;
		uint32_t	mask = uint32_t(~0);
		uint32_t	customIndex = 0;
	};

	struct BVHEntry : BVHInput {
		uint32_t	meshID		= 0;
	};
					
	struct BLASEntry {
		VKQuick::Buffer buffer;
		vk::AccelerationStructureBuildGeometryInfoKHR	buildInfo;
		vk::AccelerationStructureBuildSizesInfoKHR		sizeInfo;
		vk::UniqueAccelerationStructureKHR				accelStructure;

		std::vector<vk::AccelerationStructureBuildRangeInfoKHR>	ranges;
		std::vector<vk::AccelerationStructureGeometryKHR>		geometries;
		std::vector<uint32_t> maxPrims;
	};

	class BVHBuilder	{
	public:
		BVHBuilder(vk::Device inDevice, VKQuick::MemoryManager& inAllocator);
		~BVHBuilder();

		BVHBuilder& WithObject(BVHInput input);

		BVHBuilder& WithCommandQueue(vk::Queue inQueue);
		BVHBuilder& WithCommandPool(vk::CommandPool inPool);

		BVHBuilder& WithFlags(vk::BuildAccelerationStructureFlagsKHR flags);

		vk::UniqueAccelerationStructureKHR Build(const std::string& debugName = "");
	protected:

		void BuildBLAS(vk::BuildAccelerationStructureFlagsKHR flags);
		void BuildTLAS(vk::BuildAccelerationStructureFlagsKHR flags);

		vk::BuildAccelerationStructureFlagsKHR m_flags;

		std::map<VKQuick::Mesh*, uint32_t> m_uniqueMeshes;

		std::vector<BVHEntry> m_entries;
		std::vector<VKQuick::Mesh*>	m_meshes;

		std::vector<vk::TransformMatrixKHR>		m_transforms;
		std::vector< BLASEntry>		m_blasBuildInfo;

		vk::Queue		m_queue;
		vk::CommandPool m_pool;
		vk::Device		m_device;

		VKQuick::MemoryManager& m_memoryManager;

		vk::UniqueAccelerationStructureKHR	m_tlas;
		VKQuick::Buffer						m_tlasBuffer;
	};
}