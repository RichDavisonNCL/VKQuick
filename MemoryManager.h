/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Buffer.h"
#include "Texture.h"

namespace VKQuick {
	enum class DiscardMode {
		Immediate,
		Deferred
	};

	class MemoryManager {
	public:
		virtual ~MemoryManager() {};

		virtual Buffer	CreateBuffer(vk::BufferCreateInfo createInfo, vk::MemoryPropertyFlags memoryProperties, const std::string& debugName = "") = 0;

		virtual Buffer	CreateStagingBuffer(size_t size, const std::string& debugName = "")						= 0;
		virtual void			DiscardBuffer(Buffer& buffer, DiscardMode discard = DiscardMode::Deferred)		= 0;

		virtual void*			MapBuffer(const Buffer& buffer)		= 0;
		virtual void			UnmapBuffer(const Buffer& buffer)	= 0;
		virtual void			CopyData(const Buffer& buffer, void* data, size_t size, size_t offset = 0) = 0;

		virtual void			Update() = 0;

		virtual vk::Image		CreateImage(vk::ImageCreateInfo createInfo, const std::string& debugName = "")		= 0;
		virtual void			DiscardImage(vk::Image& img, DiscardMode discard = DiscardMode::Deferred)			= 0;

		void SetDefaultSharedBufferFamilies(const std::vector<uint32_t>& families) {
			defaultBufferFamilies = families;
		}

		void SetDefaultSharedImageFamilies(const std::vector<uint32_t>& families) {
			defaultImageFamilies = families;
		}
	
	protected:
		Buffer	AllocateBuffer() {
			Buffer b;
			b.m_sourceManager = this;
			return b;
		}

		uint32_t GetBufferID(const Buffer& b) const {
			return b.m_bufferID;
		}

		void	SetBufferID(Buffer& b, uint32_t id) {
			b.m_bufferID = id;
		}


		std::vector<uint32_t> defaultBufferFamilies;
		std::vector<uint32_t> defaultImageFamilies;
	};
}