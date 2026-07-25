/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////

vk::UniqueCommandBuffer	USEDEVICENAMESPACE CmdBufferCreate(USEDEVICESIGNATURE vk::CommandPool fromPool, const std::string& debugName) {
	std::vector<vk::UniqueCommandBuffer> buffers = device.allocateCommandBuffersUnique(
		{
			.commandPool = fromPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		}
	);

	if (!debugName.empty()) {
		SetDebugName(USEDEVICEPARAMETER * buffers[0], debugName);
	}
	return std::move(buffers[0]);
}

vk::UniqueCommandBuffer	USEDEVICENAMESPACE CmdBufferCreateBegin(USEDEVICESIGNATURE vk::CommandPool fromPool, const std::string& debugName) {
	vk::UniqueCommandBuffer buffer = CmdBufferCreate(USEDEVICEPARAMETER fromPool, debugName);
	vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
	buffer->begin(beginInfo);
	return std::move(buffer);
}

void	USEDEVICENAMESPACE WriteDescriptor(USEDEVICESIGNATURE vk::WriteDescriptorSet setInfo, vk::DescriptorBufferInfo bufferInfo) {
	setInfo.descriptorCount = 1;
	setInfo.pBufferInfo = &bufferInfo;
	if (bufferInfo.range == 0) {
		bufferInfo.range = VK_WHOLE_SIZE;
	}
	device.updateDescriptorSets(1, &setInfo, 0, nullptr);
}

void	USEDEVICENAMESPACE WriteDescriptor(USEDEVICESIGNATURE vk::WriteDescriptorSet setInfo, vk::DescriptorImageInfo imageInfo) {
	setInfo.descriptorCount = 1;
	setInfo.pImageInfo = &imageInfo;
	device.updateDescriptorSets(1, &setInfo, 0, nullptr);
}

void	USEDEVICENAMESPACE WriteCombinedImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, uint32_t subIndex, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout layout) {
	vk::DescriptorImageInfo imageInfo{
		.sampler = sampler,
		.imageView = view,
		.imageLayout = layout
	};

	vk::WriteDescriptorSet descriptorWrite{
		.dstSet = set,
		.dstBinding = bindingNum,
		.dstArrayElement = subIndex,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eCombinedImageSampler,
		.pImageInfo = &imageInfo
	};

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void	USEDEVICENAMESPACE WriteCombinedImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout layout) {
	WriteCombinedImageDescriptor(USEDEVICEPARAMETER set, bindingNum, 0, view, sampler, layout);
}

void	USEDEVICENAMESPACE WriteImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, uint32_t subIndex, vk::ImageView view, vk::ImageLayout layout) {
	vk::DescriptorImageInfo imageInfo{
		.imageView = view,
		.imageLayout = layout
	};

	vk::WriteDescriptorSet descriptorWrite{
		.dstSet = set,
		.dstBinding = bindingNum,
		.dstArrayElement = subIndex,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eSampledImage,
		.pImageInfo = &imageInfo
	};

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void	USEDEVICENAMESPACE WriteImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, vk::ImageView view, vk::ImageLayout layout) {
	WriteImageDescriptor(USEDEVICEPARAMETER set, bindingNum, 0, view, layout);
}

void	USEDEVICENAMESPACE WriteSamplerDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, uint32_t subIndex, vk::Sampler sampler) {
	vk::DescriptorImageInfo imageInfo{
		.sampler = sampler,
	};

	vk::WriteDescriptorSet descriptorWrite{
		.dstSet = set,
		.dstBinding = bindingNum,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eSampler,
		.pImageInfo = &imageInfo
	};

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void	USEDEVICENAMESPACE WriteSamplerDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, vk::Sampler sampler) {
	WriteSamplerDescriptor(USEDEVICEPARAMETER set, bindingNum, 0, sampler);
}

void	USEDEVICENAMESPACE WriteStorageImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, uint32_t subIndex, vk::ImageView view, vk::ImageLayout layout) {
	vk::DescriptorImageInfo imageInfo{
		.imageView = view,
		.imageLayout = layout
	};

	vk::WriteDescriptorSet descriptorWrite{
		.dstSet = set,
		.dstBinding = bindingNum,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eStorageImage,
		.pImageInfo = &imageInfo
	};

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void	USEDEVICENAMESPACE WriteStorageImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, vk::ImageView view, vk::ImageLayout layout) {
	WriteStorageImageDescriptor(USEDEVICEPARAMETER set, bindingNum, 0, view, layout);
}

void	USEDEVICENAMESPACE WriteBufferDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingSlot, uint32_t subIndex, vk::DescriptorType bufferType, vk::Buffer buff, std::size_t offset, std::size_t range) {
	vk::DescriptorBufferInfo descriptorInfo{
		.buffer = buff,
		.offset = offset,
		.range = range
	};

	vk::WriteDescriptorSet descriptorWrite{
		.dstSet = set,
		.dstBinding = bindingSlot,
		.dstArrayElement = subIndex,
		.descriptorCount = 1,
		.descriptorType = bufferType,
		.pBufferInfo = &descriptorInfo
	};

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void	USEDEVICENAMESPACE WriteBufferDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingSlot, vk::DescriptorType bufferType, vk::Buffer buff, std::size_t offset, std::size_t range) {
	WriteBufferDescriptor(USEDEVICEPARAMETER set, bindingSlot, 0, bufferType, buff, offset, range);
}

void	USEDEVICENAMESPACE WriteInlineUniformDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingSlot, void* data, std::size_t byteCount) {
	vk::WriteDescriptorSetInlineUniformBlock inlineWrite = {
		.dataSize = (uint32_t)byteCount,
		.pData = &data
	};

	vk::WriteDescriptorSet descriptorWrite = {
		.pNext = &inlineWrite,
		.dstSet = set,
		.dstBinding = bindingSlot,
		.dstArrayElement = 0,
		.descriptorCount = (uint32_t)byteCount,
		.descriptorType = vk::DescriptorType::eInlineUniformBlock,
	};

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void	USEDEVICENAMESPACE WriteTLASDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingSlot, vk::AccelerationStructureKHR tlas) {
	vk::WriteDescriptorSetAccelerationStructureKHR descriptorInfo{
		.accelerationStructureCount = 1,
		.pAccelerationStructures = &tlas
	};

	vk::WriteDescriptorSet descriptorWrite{
		.pNext = &descriptorInfo,
		.dstSet = set,
		.dstBinding = bindingSlot,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eAccelerationStructureKHR,
	};

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

vk::UniqueDescriptorSet USEDEVICENAMESPACE CreateDescriptorSet(USEDEVICESIGNATURE vk::DescriptorPool pool, vk::DescriptorSetLayout  layout) {
	vk::DescriptorSetAllocateInfo allocateInfo{
		.descriptorPool = pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &layout
	};
	return std::move(device.allocateDescriptorSetsUnique(allocateInfo)[0]);
}

vk::UniqueDescriptorSet USEDEVICENAMESPACE CreateDescriptorSet(USEDEVICESIGNATURE vk::DescriptorPool pool, vk::DescriptorSetLayout  layout, uint32_t variableDescriptorCount) {
	vk::DescriptorSetVariableDescriptorCountAllocateInfoEXT variableDescriptorInfo{
		.descriptorSetCount = 1,
		.pDescriptorCounts = &variableDescriptorCount
	};
	vk::DescriptorSetAllocateInfo allocateInfo{
		.pNext = (const void*)&variableDescriptorInfo,
		.descriptorPool = pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &layout,
	};
	return std::move(device.allocateDescriptorSetsUnique(allocateInfo)[0]);
}

vk::UniqueSemaphore USEDEVICENAMESPACE CreateTimelineSemaphore(USEDEVICESIGNATURE uint64_t initialValue) {
	vk::SemaphoreTypeCreateInfo typeCreateInfo{
		.semaphoreType = vk::SemaphoreType::eTimeline,
		.initialValue = initialValue
	};
	vk::SemaphoreCreateInfo createInfo{
		.pNext = &typeCreateInfo,
	};
	return std::move(device.createSemaphoreUnique(createInfo));
}

vk::Result	USEDEVICENAMESPACE TimelineSemaphoreHostWait(USEDEVICESIGNATURE vk::Semaphore semaphore, uint64_t waitVal, uint64_t waitTime) {
	vk::SemaphoreWaitInfo waitInfo{
		.semaphoreCount = 1,
		.pSemaphores = &semaphore,
		.pValues = &waitVal
	};
	return device.waitSemaphores(waitInfo, UINT64_MAX);
}

void	USEDEVICENAMESPACE TimelineSemaphoreHostSignal(USEDEVICESIGNATURE vk::Semaphore semaphore, uint64_t signalVal) {
	vk::SemaphoreSignalInfo signalInfo{
		.semaphore = semaphore,
		.value = signalVal
	};
	device.signalSemaphore(signalInfo);
}

/*Descriptor Buffer Writing*/
void USEDEVICENAMESPACE WriteBufferDescriptor(USEDEVICESIGNATURE
	const vk::PhysicalDeviceDescriptorBufferPropertiesEXT& props,
	void* descriptorBufferMemory,
	vk::DescriptorSetLayout layout,
	size_t layoutIndex,
	vk::DeviceAddress bufferAddress,
	size_t bufferSize
) {
	vk::DescriptorAddressInfoEXT address{
		.address = bufferAddress,
		.range = bufferSize
	};
	vk::DescriptorGetInfoEXT getInfo{
		.type = vk::DescriptorType::eUniformBuffer,
		.data = &address
	};

	vk::DeviceSize offset = device.getDescriptorSetLayoutBindingOffsetEXT(layout, layoutIndex);

	device.getDescriptorEXT(&getInfo, props.uniformBufferDescriptorSize, ((char*)descriptorBufferMemory) + offset);
}