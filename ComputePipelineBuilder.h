/******************************************************************************
This file is part of the QuickVK library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Pipeline.h"
#include "ShaderModule.h"
#include "PipelineBuilderBase.h"
#include "SmartTypes.h"

namespace QuickVK {
	/*
	ComputePipelineBuilder: A Builder class to automate the creation of 
	compute pipelines, including the correct push constants and descriptor
	set layouts, obtained from the shader module via reflection. 
	*/
	class ComputePipelineBuilder : public PipelineBuilderBase<ComputePipelineBuilder, vk::ComputePipelineCreateInfo>	{
	public:
		ComputePipelineBuilder(vk::Device m_device);
		~ComputePipelineBuilder() {}

		ComputePipelineBuilder& WithShaderBinary(const std::string& filename, const std::string& entrypoint = "main");
		ComputePipelineBuilder& WithShaderModule(const ShaderModule& module, const std::string& entrypoint = "main");

		Pipeline	Build(const std::string& debugName = "", vk::PipelineCache cache = {});

	protected:
		std::string			m_entryPoint;
	};
};