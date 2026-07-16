/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Instance.h"
#include "PipelineBuilderBase.h"


namespace VKQuick {
	namespace BindingTableOrder {
		enum Type : uint32_t {
			RayGen,
			Miss,
			Hit,
			Call,
			MAX_SIZE
		};
	};

	struct ShaderBindingTable {
		Buffer tableBuffer;
		vk::StridedDeviceAddressRegionKHR regions[BindingTableOrder::MAX_SIZE];
	};

	class ShaderBindingTableBuilder {
	public:
		ShaderBindingTableBuilder(const std::string& debugName = "");
		~ShaderBindingTableBuilder() = default;

		ShaderBindingTableBuilder& WithProperties(vk::PhysicalDeviceRayTracingPipelinePropertiesKHR properties);

		ShaderBindingTableBuilder& WithPipeline(vk::Pipeline pipe, const vk::RayTracingPipelineCreateInfoKHR& createInfo);

		ShaderBindingTableBuilder& WithLibrary(const vk::RayTracingPipelineCreateInfoKHR& createInfo);

		ShaderBindingTable Build(vk::Device device, MemoryManager& memManager);

	protected:
		void FillCounts(const vk::RayTracingPipelineCreateInfoKHR* fromInfo);

		vk::PhysicalDeviceRayTracingPipelinePropertiesKHR properties;

		const vk::RayTracingPipelineCreateInfoKHR* pipeCreateInfo;

		std::vector<const vk::RayTracingPipelineCreateInfoKHR*> libraries;

		vk::Pipeline pipeline;

		std::string debugName;

		uint32_t handleCounts[BindingTableOrder::MAX_SIZE] = { };
	};
}