/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Instance.h"
#include "PipelineBuilderBase.h"
#include "RayPipeline.h"

namespace VKQuick {

	class RayTracingPipelineBuilder : 
		public PipelineBuilderBase< RayTracingPipelineBuilder, vk::RayTracingPipelineCreateInfoKHR> {
	public:
		RayTracingPipelineBuilder(vk::Device device, vk::PhysicalDevice physicalDevice, MemoryManager& memManager);
		~RayTracingPipelineBuilder();

		RayTracingPipelineBuilder& WithTriangleHitGroup(uint32_t closestHit = VK_SHADER_UNUSED_KHR, uint32_t anyHit = VK_SHADER_UNUSED_KHR);
		RayTracingPipelineBuilder& WithProceduralHitGroup(uint32_t intersection, uint32_t closestHit = VK_SHADER_UNUSED_KHR, uint32_t anyHit = VK_SHADER_UNUSED_KHR);

		RayTracingPipelineBuilder& WithRecursionDepth(uint32_t count);

		RayPipeline Build(const std::string& debugName = "", vk::PipelineCache cache = {});

	protected:
		void FillSBTCounts(const vk::RayTracingPipelineCreateInfoKHR* fromInfo);
		void CreateSBT(RayPipeline& pipeline, const std::string& debugName = "");

		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_genGroups;
		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_missGroups;
		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_hitGroups;
		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_allGroups;

		uint32_t handleCounts[BindingTableOrder::MAX_SIZE] = { };

		vk::PhysicalDevice	m_physicalDevice;
		MemoryManager&		m_memoryManager;

		vk::PipelineDynamicStateCreateInfo					m_dynamicCreate;
	};
}