/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
//#include "../NCLCoreClasses/Texture.h"
//#include "TextureBuilder.h"
#include "SmartTypes.h"

namespace VKQuick {
	class MemoryManager;

	struct LoadedTexture {
		char* texData;
		vk::Extent3D dimensions = { 0, 0, 1 };
		uint32_t channels	= 0;
		uint32_t flags		= 0;
	};

	using TextureLoadFunction			= std::function<LoadedTexture(const std::string& filename)>;
	using TextureLoadReleaseFunction	= std::function<void(LoadedTexture& texture)>;

	class Texture {
		friend class TextureBuilder;
	public:
		~Texture();

		vk::ImageView GetDefaultView() const {
			return *m_defaultView;
		}

		vk::Format GetFormat() const {
			return m_createInfo.format;
		}

		vk::Image GetImage() const {
			return m_image;
		}

		//Allows us to pass a texture as vk type to various functions
		operator vk::Image() const {
			return m_image;
		}
		operator vk::ImageView() const {
			return *m_defaultView;
		}		
		operator vk::Format() const {
			return m_createInfo.format;
		}

		void GenerateMipMaps(	vk::CommandBuffer  buffer, 
								vk::ImageLayout startLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
								vk::ImageLayout endLayout	= vk::ImageLayout::eShaderReadOnlyOptimal, 
								vk::PipelineStageFlags2 endFlags = vk::PipelineStageFlagBits2::eFragmentShader);

		static size_t GetMaxMips(vk::Extent3D extent) {
			uint32_t maxSize = extent.width > extent.height ? extent.width : extent.height;
			maxSize = maxSize > extent.depth ? maxSize : extent.depth;
			return (size_t)std::floor(log2((float(maxSize)))) + 1;
		}

	protected:
		Texture();

		vk::ImageCreateInfo		m_createInfo;

		vk::UniqueImageView		m_defaultView;
		vk::Image				m_image; //Don't use 'Unique', must use the memory manager

		vk::ImageAspectFlags	m_aspectType;

		vk::UniqueSemaphore		m_workSemaphore;

		MemoryManager*			m_memManager = nullptr;
	};
}