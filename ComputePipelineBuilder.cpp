/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "ComputePipelineBuilder.h"
#include "Utils.h"

using namespace VKQuick;

ComputePipelineBuilder::ComputePipelineBuilder(vk::Device device) : PipelineBuilderBase(device){
}

Pipeline	ComputePipelineBuilder::Build(const std::string& debugName, vk::PipelineCache cache) {
	Pipeline output;
	assert(!m_usedModules.empty());

	FillShaderLayouts(output);

	vk::PipelineShaderStageCreateInfo	m_createInfo;
	m_createInfo.stage	= vk::ShaderStageFlagBits::eCompute;
	m_createInfo.module = m_usedModules[0]->GetShaderModule();
	m_createInfo.pName	= m_moduleEntryPoints[0].c_str();

	m_pipelineCreate.setLayout(*output.layout);
	m_pipelineCreate.setStage(m_createInfo);

	vk::PipelineCreateFlags2CreateInfo pipeFlags;
	if (m_pipelineCreateBits) {
		pipeFlags.flags = m_pipelineCreateBits;
		m_pipelineCreate.pNext = &pipeFlags;
	}

	output.pipeline = m_sourceDevice.createComputePipelineUnique(cache, m_pipelineCreate).value;

	if (!debugName.empty()) {
		SetDebugName(m_sourceDevice, *output.pipeline, debugName);
	}

	return output;
}