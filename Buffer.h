/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once

namespace VKQuick {
	class MemoryManager;
	//A buffer, backed by memory we have allocated elsewhere
	class Buffer {
	public:		
		friend class MemoryManager;
		Buffer();
		Buffer(Buffer&& obj);
		virtual ~Buffer();
		Buffer& operator=(Buffer&& obj);

		//A convenience func to help get around vma holding various
		//mapped pointers etc, so us calling mapBuffer can cause
		//validation errors
		virtual void	CopyData(void* data, size_t size) const;

		virtual void*	Map()		const;
		virtual void	Unmap()		const;

		template<typename T>
		T* Map() const {
			void* data = Map();
			return static_cast<T*>(data);
		}

		//Convenience function so we can use this class in place of a vkBuffer when necessary
		operator vk::Buffer() const {
			return buffer;
		}

		vk::Buffer GetHandle() const {
			return buffer;
		}

		vk::DeviceAddress GetDeviceAddress() const {
			return deviceAddress;
		}

		size_t GetSize() const {
			return size;
		}

	protected:
		uint32_t GetBufferID() const {
			return m_bufferID;
		}

		void	SetBufferID(uint32_t id) {
			m_bufferID = id;
		}

		MemoryManager* m_sourceManager = nullptr;

		uint32_t m_bufferID;

		vk::Buffer	buffer;
		size_t		size = 0;

		vk::DeviceAddress	deviceAddress;
	};

	using UniqueBuffer = std::unique_ptr<Buffer>;
	using SharedBuffer = std::shared_ptr<Buffer>;
};