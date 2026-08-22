/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once

namespace VKQuick {
	class ShaderModule  {
	public:
		ShaderModule(const std::string& filename, vk::Device device);

		~ShaderModule() = default;
		void CombineLayoutBindings(std::vector<std::vector<vk::DescriptorSetLayoutBinding>>& inoutBindings, vk::ShaderStageFlags layoutStage)	const;
		void CombinePushConstantRanges(std::vector< vk::PushConstantRange>& inoutRanges, vk::ShaderStageFlags layoutStage)						const;

	//protected:		
		void AddReflectionData(const std::vector<char>& data);

		std::vector<std::vector<vk::DescriptorSetLayoutBinding>>	m_allLayoutsBindings;

		std::vector< vk::PushConstantRange>		m_pushConstants;

		vk::UniqueShaderModule				m_shaderModule;
		std::string							m_fileName;
	};

	using UniqueShaderModule = std::unique_ptr<ShaderModule>;
	using SharedShaderModule = std::shared_ptr<ShaderModule>;
}