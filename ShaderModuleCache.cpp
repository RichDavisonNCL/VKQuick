/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "ShaderModuleCache.h"

using namespace VKQuick;

ShaderModuleCache::ShaderModuleCache(vk::Device device) {
	m_sourceDevice = device;
}

VKQuick::ShaderModule* ShaderModuleCache::GetCachedModule(const std::string& name) const {
	for (auto& i : m_shaderModules) {
		if (i->m_fileName == name) {
			return i.get();
		}
	}
	return nullptr;
}

VKQuick::ShaderModule* ShaderModuleCache::AddCachedModule(const std::string& name) {
	m_shaderModules.push_back(std::make_unique<ShaderModule>(name, m_sourceDevice));
	return m_shaderModules.back().get();
}