/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "Buffer.h"
#include "MemoryManager.h"

using namespace VKQuick;

Buffer::Buffer() {
	buffer = nullptr;
	size = 0;
	deviceAddress = 0;
}

Buffer::Buffer(Buffer&& obj)  {
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

Buffer& Buffer::operator=(Buffer&& obj) {
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

Buffer::~Buffer() {
	if (buffer && m_sourceManager) {
		m_sourceManager->DiscardBuffer(*this);
	}
}

void Buffer::CopyData(void* data, size_t size) const {
	m_sourceManager->CopyData(*this, data, size);
}

void* Buffer::Map() const {
	return m_sourceManager->MapBuffer(*this);
}

void	Buffer::Unmap() const {
	m_sourceManager->UnmapBuffer(*this);
}