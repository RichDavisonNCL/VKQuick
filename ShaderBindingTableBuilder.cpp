/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "ShaderBindingTableBuilder.h"
#include "Buffer.h"

using namespace VKQuick;

ShaderBindingTableBuilder::ShaderBindingTableBuilder(vk::Device device, MemoryManager& memManager) {
	m_sourceDevice	= device;
	m_memManager	= &memManager;
}

ShaderBindingTableBuilder& ShaderBindingTableBuilder::WithProperties(vk::PhysicalDeviceRayTracingPipelinePropertiesKHR deviceProps) {
	properties = deviceProps;
	return *this;
}

ShaderBindingTableBuilder& ShaderBindingTableBuilder::WithPipeline(vk::Pipeline inPipe, const vk::RayTracingPipelineCreateInfoKHR& inPipeCreateInfo) {
	pipeline = inPipe;
	pipeCreateInfo = &inPipeCreateInfo;
	return *this;
}

ShaderBindingTableBuilder& ShaderBindingTableBuilder::WithLibrary(const vk::RayTracingPipelineCreateInfoKHR& createInfo) {
	libraries.push_back(&createInfo);
	return *this;
}

void ShaderBindingTableBuilder::FillCounts(const vk::RayTracingPipelineCreateInfoKHR* fromInfo) {
	for (int i = 0; i < fromInfo->groupCount; ++i) {
		if (fromInfo->pGroups[i].type == vk::RayTracingShaderGroupTypeKHR::eGeneral) {
			int shaderType = fromInfo->pGroups[i].generalShader;

			if (fromInfo->pStages[shaderType].stage == vk::ShaderStageFlagBits::eRaygenKHR) {
				handleCounts[BindingTableOrder::RayGen]++;
			}
			else if (fromInfo->pStages[shaderType].stage == vk::ShaderStageFlagBits::eMissKHR) {
				handleCounts[BindingTableOrder::Miss]++;
			}
			else if(fromInfo->pStages[shaderType].stage == vk::ShaderStageFlagBits::eCallableKHR) {
				handleCounts[BindingTableOrder::Call]++;
			}
		}
		else { //Must be a hit group
			handleCounts[BindingTableOrder::Hit]++;
		}
	}
}

int MakeMultipleOf(int input, int multiple) {
	int count = input / multiple;
	int r = input % multiple;
	
	if (r != 0) {
		count += 1;
	}

	return count * multiple;
}

ShaderBindingTable ShaderBindingTableBuilder::Build(const std::string& debugName) {
	assert(pipeCreateInfo);
	assert(pipeline);

	ShaderBindingTable table;

	FillCounts(pipeCreateInfo); //Fills the handleIndices vectors

	uint32_t numShaderGroups = pipeCreateInfo->groupCount;
	for (auto& i : libraries) {
		numShaderGroups += i->groupCount;
		FillCounts(i);
	}

	uint32_t handleSize			= properties.shaderGroupHandleSize;
	uint32_t alignedHandleSize	= MakeMultipleOf(handleSize, properties.shaderGroupHandleAlignment);
	uint32_t totalHandleSize	= numShaderGroups * handleSize;

	std::vector<uint8_t> handles(totalHandleSize);
	auto result = m_sourceDevice.getRayTracingShaderGroupHandlesKHR(pipeline, 0, numShaderGroups, totalHandleSize, handles.data());

	uint32_t bufferSize = 0;

	for (int i = 0; i < BindingTableOrder::MAX_SIZE; ++i) {
		table.regions[i].size	= MakeMultipleOf(alignedHandleSize * handleCounts[i], properties.shaderGroupBaseAlignment);
		table.regions[i].stride = alignedHandleSize;
		bufferSize += table.regions[i].size;
	}
	table.regions[0].stride = table.regions[0].size;

	table.tableBuffer = m_memManager->CreateBuffer(
		{
			.size = bufferSize,
			.usage =	vk::BufferUsageFlagBits::eShaderDeviceAddress		|
						vk::BufferUsageFlagBits::eTransferSrc				|
						vk::BufferUsageFlagBits::eShaderDeviceAddressKHR	|
						vk::BufferUsageFlagBits::eShaderBindingTableKHR,
		},
		vk::MemoryPropertyFlagBits::eHostVisible,
		debugName + " SBT Buffer"
	);


	vk::DeviceAddress bufferAddress = m_sourceDevice.getBufferAddress({ .buffer = table.tableBuffer.buffer });

	char* bufferData = (char*)table.tableBuffer.Map();
	int dataOffset = 0;
	int currentHandleIndex = 0;
	for (int i = 0; i < BindingTableOrder::MAX_SIZE; ++i) { //For each group type
		int dataOffsetStart = dataOffset;
		table.regions[i].deviceAddress = bufferAddress + dataOffsetStart;
	
		for (int j = 0; j < handleCounts[i]; ++j) { //For entries in that group
			memcpy(bufferData + dataOffset, handles.data() + (currentHandleIndex++ * handleSize), handleSize);
			dataOffset += alignedHandleSize;
		}
		dataOffset = dataOffsetStart + table.regions[i].size;
	}

	table.tableBuffer.Unmap();

	return table;
}