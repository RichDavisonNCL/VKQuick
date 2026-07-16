/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MemoryManager.h"
#include "Buffer.h"
#include "Texture.h"

#include "vma/vk_mem_alloc.h"

namespace VKQuick {
	struct	VKQuickInitialisation;

	class VMAMemoryManager : public MemoryManager {
	public:
		VMAMemoryManager(vk::Device device, vk::PhysicalDevice physicalDevice, vk::Instance instance, const VKQuickInitialisation& vkInit);
		virtual ~VMAMemoryManager();

		Buffer	CreateBuffer(vk::BufferCreateInfo createInfo, vk::MemoryPropertyFlags flags, const std::string& debugName = "")	override;
		Buffer	CreateStagingBuffer(size_t size, const std::string& debugName = "")						override;
		void			DiscardBuffer(Buffer& buffer, DiscardMode discard)								override;

		void*			MapBuffer(const Buffer& buffer)			override;
		void			UnmapBuffer(const Buffer& buffer)		override;
		void			CopyData(const Buffer& buffer, void* data, size_t size, size_t offset = 0) override;

		vk::Image		CreateImage(vk::ImageCreateInfo createInfo, const std::string& debugName = "")		override;
		void			DiscardImage(vk::Image& tex, DiscardMode discard)									override;

		void			Update() override;

	protected:
		uint32_t	GetSpareBufferID();
		void		DeleteBuffer(Buffer& buffer);

		struct DeferredBufferDeletion {
			Buffer	buffer;
			uint32_t		framesCount;
		};

		struct Allocation {
			VmaAllocation			m_allocationHandle	= {};
			VmaAllocationInfo		m_allocationInfo	= {};
		};

		std::map<vk::Image, Allocation>		m_imageAllocations;

		std::vector<Allocation>				m_bufferAllocations;
		std::vector<uint32_t>				m_spareBufferIDs;

		std::vector<DeferredBufferDeletion> m_deferredDeleteBuffers;

		VmaAllocator			m_memoryAllocator;
		VmaAllocatorCreateInfo	m_allocatorInfo;

		uint32_t m_framesInFlight;
	};


}