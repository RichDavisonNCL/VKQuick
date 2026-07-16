/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once

namespace VKQuick {
	class Mesh;
	class Texture;
	class Buffer;
	class ShaderModule;

	using UniqueMesh		= std::unique_ptr<Mesh>;
	using SharedMesh		= std::shared_ptr<Mesh>;

	using UniqueTexture	= std::unique_ptr<Texture>;
	using SharedTexture	= std::shared_ptr<Texture>;

	using UniqueBuffer	= std::unique_ptr<Buffer>;
	using SharedBuffer	= std::shared_ptr<Buffer>;

	using UniqueShaderModule = std::unique_ptr<ShaderModule>;
	using SharedShaderModule = std::shared_ptr<ShaderModule>;
}