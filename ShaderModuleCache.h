/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ShaderModule.h"

namespace VKQuick {
	class ShaderModuleCache  {
	public:
		ShaderModuleCache(vk::Device device);
		~ShaderModuleCache();

		VKQuick::ShaderModule* GetCachedModule(const std::string& name);
	private:
		vk::Device			m_sourceDevice;

		std::unordered_map<std::string, ShaderModule*> m_shaderModules;
	};

	using UniqueShaderModuleCache = std::unique_ptr<ShaderModuleCache>;
	using SharedShaderModuleCache = std::shared_ptr<ShaderModuleCache>;
}