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

ShaderModuleCache::~ShaderModuleCache() {
	for (auto& i : m_shaderModules) {
		delete i.second;
	}
}

VKQuick::ShaderModule* ShaderModuleCache::GetCachedModule(const std::string& name) {
	auto i = m_shaderModules.find(name);
	if (i == m_shaderModules.end()) {
		auto j = m_shaderModules.insert({ name , new ShaderModule(name, m_sourceDevice) });
		return j.first->second;
	}
	return i->second;
}