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

		VKQuick::ShaderModule* GetCachedModule(const std::string& name) const;

		VKQuick::ShaderModule* AddCachedModule(const std::string& name);
		private:
			vk::Device			m_sourceDevice;

			std::vector<UniqueShaderModule> m_shaderModules;
	};
}