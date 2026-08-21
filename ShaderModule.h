/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../NCLCoreClasses/Shader.h"

namespace VKQuick {
	class ShaderModule  {
	public:
		ShaderModule(const std::string& filename,  vk::ShaderStageFlagBits moduleStage, vk::Device device);

		~ShaderModule() = default;
		void CombineLayoutBindings(std::vector<std::vector<vk::DescriptorSetLayoutBinding>>& inoutBindings, vk::ShaderStageFlags layoutStage = {})		const;
		void CombinePushConstantRanges(std::vector< vk::PushConstantRange>& inoutRanges)						const;

	//protected:		
		void AddReflectionData(const std::vector<char>& data, vk::ShaderStageFlags stage);

		std::vector<std::vector<vk::DescriptorSetLayoutBinding>>	m_allLayoutsBindings;

		std::vector< vk::PushConstantRange>		m_pushConstants;

		vk::UniqueShaderModule				m_shaderModule;
		vk::ShaderStageFlagBits				m_shaderStage;
		std::string							m_fileName;
	};

	using UniqueShaderModule = std::unique_ptr<ShaderModule>;
	using SharedShaderModule = std::shared_ptr<ShaderModule>;
}