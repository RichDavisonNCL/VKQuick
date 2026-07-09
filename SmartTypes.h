/******************************************************************************
This file is part of the QuickVK library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once

namespace QuickVK {
	class VulkanMesh;
	class VulkanTexture;
	class Buffer;
	class VulkanShaderModule;

	using UniqueVulkanMesh		= std::unique_ptr<VulkanMesh>;
	using SharedVulkanMesh		= std::shared_ptr<VulkanMesh>;

	using UniqueVulkanTexture	= std::unique_ptr<VulkanTexture>;
	using SharedVulkanTexture	= std::shared_ptr<VulkanTexture>;

	using UniqueBuffer	= std::unique_ptr<Buffer>;
	using SharedBuffer	= std::shared_ptr<Buffer>;

	using UniqueVulkanShaderModule = std::unique_ptr<VulkanShaderModule>;
	using SharedVulkanShaderModule = std::shared_ptr<VulkanShaderModule>;
}