/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Utils.h"

namespace VKQuick {
	class DescriptorSetBuilder {
	public:
		DescriptorSetBuilder(vk::Device device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint32_t variableDescriptorCount = 0) {
			m_device	= device;
			m_layout	= layout;

			vk::DescriptorSetVariableDescriptorCountAllocateInfoEXT variableDescriptorInfo{
				.descriptorSetCount = 1,
				.pDescriptorCounts	= &variableDescriptorCount
			};

			vk::DescriptorSetAllocateInfo allocateInfo{
				.descriptorPool		= pool,
				.descriptorSetCount = 1,
				.pSetLayouts		= &layout
			};

			if (variableDescriptorCount > 0) {
				allocateInfo.pNext = (const void*)&variableDescriptorInfo;
			};

			m_set = std::move(device.allocateDescriptorSetsUnique(allocateInfo)[0]);
		}
		~DescriptorSetBuilder() = default;

		DescriptorSetBuilder& WriteSampler(uint32_t binding, vk::Sampler sampler) {
			VKQuick::WriteSamplerDescriptor(m_device, *m_set, binding, sampler);
			return *this;
		}

		DescriptorSetBuilder& WriteImage(uint32_t binding, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal) {
			VKQuick::WriteCombinedImageDescriptor(m_device, *m_set, binding, view, sampler, layout);
			return *this;
		}

		DescriptorSetBuilder& WriteImage(uint32_t binding, vk::ImageView view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal) {
			VKQuick::WriteImageDescriptor(m_device, *m_set, binding, view, layout);
			return *this;
		}

		DescriptorSetBuilder& WriteStorageImage(uint32_t binding, vk::ImageView view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal) {
			VKQuick::WriteStorageImageDescriptor(m_device, *m_set, binding, view, layout);
			return *this;
		}

		DescriptorSetBuilder& WriteBuffer(uint32_t binding, vk::Buffer buffer, vk::DescriptorType type, size_t offset = 0, size_t range = VK_WHOLE_SIZE) {
			VKQuick::WriteBufferDescriptor(m_device, *m_set, binding, type, buffer, offset, range);
			return *this;
		}

		DescriptorSetBuilder& WriteTLAS(uint32_t binding, vk::AccelerationStructureKHR tlas) {
			VKQuick::WriteTLASDescriptor(m_device, *m_set, binding, tlas);
			return *this;
		}

		vk::UniqueDescriptorSet Build() {
			return std::move(m_set);
		}

	protected:
		vk::Device				m_device;
		vk::DescriptorSetLayout	m_layout;
		vk::UniqueDescriptorSet	m_set;
	};
}