/******************************************************************************
This file is part of the QuickVK library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Utils.h"

namespace QuickVK {
	class DescriptorSetWriter {
	public:
		DescriptorSetWriter(vk::Device device, vk::DescriptorSet set) {
			m_device	= device;
			m_set		= set;
		}
		~DescriptorSetWriter() = default;

		DescriptorSetWriter& WriteSampler(uint32_t binding, vk::Sampler sampler) {
			QuickVK::WriteSamplerDescriptor(m_device, m_set, binding, sampler);
			return *this;
		}

		DescriptorSetWriter& WriteImage(uint32_t binding, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout m_layout = vk::ImageLayout::eShaderReadOnlyOptimal) {
			QuickVK::WriteCombinedImageDescriptor(m_device, m_set, binding, view, sampler, m_layout);
			return *this;
		}

		DescriptorSetWriter& WriteImage(uint32_t binding, vk::ImageView view, vk::ImageLayout m_layout = vk::ImageLayout::eShaderReadOnlyOptimal) {
			QuickVK::WriteImageDescriptor(m_device, m_set, binding, view, m_layout);
			return *this;
		}

		DescriptorSetWriter& WriteStorageImage(uint32_t binding, vk::ImageView view, vk::ImageLayout m_layout = vk::ImageLayout::eShaderReadOnlyOptimal) {
			QuickVK::WriteStorageImageDescriptor(m_device, m_set, binding, view, m_layout);
			return *this;
		}

		DescriptorSetWriter& WriteBuffer(uint32_t binding, vk::Buffer buffer, vk::DescriptorType type, size_t offset = 0, size_t range = VK_WHOLE_SIZE) {
			QuickVK::WriteBufferDescriptor(m_device, m_set, binding, type, buffer, offset, range);
			return *this;
		}

		DescriptorSetWriter& WriteTLAS(uint32_t binding, vk::AccelerationStructureKHR tlas) {
			QuickVK::WriteTLASDescriptor(m_device, m_set, binding, tlas);
			return *this;
		}

	protected:
		vk::Device			m_device;
		vk::DescriptorSet	m_set;
	};
}