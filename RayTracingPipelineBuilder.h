/******************************************************************************
This file is part of the QuickVK library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Instance.h"
#include "PipelineBuilderBase.h"

namespace QuickVK {

	class RayTracingPipelineBuilder : 
		public PipelineBuilderBase< RayTracingPipelineBuilder, vk::RayTracingPipelineCreateInfoKHR> {
	public:
		RayTracingPipelineBuilder(vk::Device m_device);
		~RayTracingPipelineBuilder();

		RayTracingPipelineBuilder& WithShaderBinary(const std::string& filename, vk::ShaderStageFlagBits stage, const std::string& entrypoint = "main");
		RayTracingPipelineBuilder& WithShaderModule(const ShaderModule& module, const std::string& entrypoint = "main");

		RayTracingPipelineBuilder& WithRayGenGroup(uint32_t shaderIndex);
		RayTracingPipelineBuilder& WithMissGroup(uint32_t shaderIndex);
		RayTracingPipelineBuilder& WithTriangleHitGroup(uint32_t closestHit = VK_SHADER_UNUSED_KHR, uint32_t anyHit = VK_SHADER_UNUSED_KHR);
		RayTracingPipelineBuilder& WithProceduralHitGroup(uint32_t intersection, uint32_t closestHit = VK_SHADER_UNUSED_KHR, uint32_t anyHit = VK_SHADER_UNUSED_KHR);

		RayTracingPipelineBuilder& WithRecursionDepth(uint32_t count);

		Pipeline Build(const std::string& debugName = "", vk::PipelineCache cache = {});

	protected:
		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_genGroups;
		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_missGroups;
		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_hitGroups;
		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_allGroups;

		vk::PipelineDynamicStateCreateInfo					m_dynamicCreate;
	};
}