/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////

template <typename T>
void USEDEVICENAMESPACE SetDebugName(USEDEVICESIGNATURE T handle, const std::string& debugName) {
	device.setDebugUtilsObjectNameEXT(
		vk::DebugUtilsObjectNameInfoEXT()
		.setObjectType(T::objectType)
		.setObjectHandle(uint64_t(static_cast<T::CType>(handle)))
		.setPObjectName(debugName.c_str())
	);
}

#ifndef USEDEVICEIGNORESIGS

vk::UniqueCommandBuffer	USEDEVICENAMESPACE CmdBufferCreate(USEDEVICESIGNATURE vk::CommandPool fromPool, const std::string& debugName);

vk::UniqueCommandBuffer	USEDEVICENAMESPACE CmdBufferCreateBegin(USEDEVICESIGNATURE vk::CommandPool fromPool, const std::string& debugName = "");

void	USEDEVICENAMESPACE WriteDescriptor(USEDEVICESIGNATURE vk::WriteDescriptorSet setInfo, vk::DescriptorBufferInfo bufferInfo);

void	USEDEVICENAMESPACE WriteDescriptor(USEDEVICESIGNATURE vk::WriteDescriptorSet setInfo, vk::DescriptorImageInfo imageInfo);

void	USEDEVICENAMESPACE WriteCombinedImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, uint32_t subIndex, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);
void	USEDEVICENAMESPACE WriteCombinedImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

void	USEDEVICENAMESPACE WriteImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, uint32_t subIndex, vk::ImageView view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

void	USEDEVICENAMESPACE WriteImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, vk::ImageView view, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

void	USEDEVICENAMESPACE WriteSamplerDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, uint32_t subIndex, vk::Sampler sampler);

void	USEDEVICENAMESPACE WriteSamplerDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, vk::Sampler sampler);

void	USEDEVICENAMESPACE WriteStorageImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, uint32_t subIndex, vk::ImageView view, vk::ImageLayout layout);

void	USEDEVICENAMESPACE WriteStorageImageDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingNum, vk::ImageView view, vk::ImageLayout layout);


void	USEDEVICENAMESPACE WriteBufferDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingSlot, uint32_t subIndex, vk::DescriptorType bufferType, vk::Buffer buff, std::size_t offset = 0, std::size_t range = VK_WHOLE_SIZE);


void	USEDEVICENAMESPACE WriteBufferDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingSlot, vk::DescriptorType bufferType, vk::Buffer buff, std::size_t offset = 0, std::size_t range = VK_WHOLE_SIZE);

void	USEDEVICENAMESPACE WriteInlineUniformDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingSlot, void* data, std::size_t byteCount);


void	USEDEVICENAMESPACE WriteTLASDescriptor(USEDEVICESIGNATURE vk::DescriptorSet set, uint32_t bindingSlot, vk::AccelerationStructureKHR tlas);


vk::UniqueDescriptorSet USEDEVICENAMESPACE CreateDescriptorSet(USEDEVICESIGNATURE vk::DescriptorPool pool, vk::DescriptorSetLayout  layout);


vk::UniqueDescriptorSet USEDEVICENAMESPACE CreateDescriptorSet(USEDEVICESIGNATURE vk::DescriptorPool pool, vk::DescriptorSetLayout  layout, uint32_t variableDescriptorCount);


vk::UniqueSemaphore USEDEVICENAMESPACE CreateTimelineSemaphore(USEDEVICESIGNATURE uint64_t initialValue);


vk::Result	USEDEVICENAMESPACE TimelineSemaphoreHostWait(USEDEVICESIGNATURE vk::Semaphore semaphore, uint64_t waitVal, uint64_t waitTime);


void	USEDEVICENAMESPACE TimelineSemaphoreHostSignal(USEDEVICESIGNATURE vk::Semaphore semaphore, uint64_t signalVal);

/*Descriptor Buffer Writing*/
	void USEDEVICENAMESPACE WriteBufferDescriptor(USEDEVICESIGNATURE
		const vk::PhysicalDeviceDescriptorBufferPropertiesEXT & props,
		void* descriptorBufferMemory,
		vk::DescriptorSetLayout layout,
		size_t layoutIndex,
		vk::DeviceAddress bufferAddress,
		size_t bufferSize
	);

#endif