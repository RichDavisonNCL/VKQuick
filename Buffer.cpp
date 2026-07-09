/******************************************************************************
This file is part of the QuickVK

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "Buffer.h"
#include "MemoryManager.h"

using namespace QuickVK;

VulkanBuffer::VulkanBuffer() {
	buffer = nullptr;
	size = 0;
	deviceAddress = 0;
}

VulkanBuffer::VulkanBuffer(VulkanBuffer&& obj)  {
	buffer			= obj.buffer;
	deviceAddress	= obj.deviceAddress;
	size			= obj.size;
	m_sourceManager	= obj.m_sourceManager;
	m_bufferID		= obj.m_bufferID;

	obj.buffer		= VK_NULL_HANDLE;
	obj.m_bufferID	= -1;
	obj.m_sourceManager = nullptr;
	obj.size		= 0;
}

VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& obj) {
	if (this != &obj) {
		buffer			= obj.buffer;
		deviceAddress	= obj.deviceAddress;
		size			= obj.size;
		m_sourceManager	= obj.m_sourceManager;
		m_bufferID		= obj.m_bufferID;

		obj.buffer		= VK_NULL_HANDLE;
		obj.m_bufferID	= -1;
		obj.m_sourceManager = nullptr;
		obj.size		= 0;
	}
	return *this;
}

VulkanBuffer::~VulkanBuffer() {
	if (buffer && m_sourceManager) {
		m_sourceManager->DiscardBuffer(*this);
	}
}

void VulkanBuffer::CopyData(void* data, size_t size) const {
	m_sourceManager->CopyData(*this, data, size);
}

void* VulkanBuffer::Map() const {
	return m_sourceManager->MapBuffer(*this);
}

void	VulkanBuffer::Unmap() const {
	m_sourceManager->UnmapBuffer(*this);
}