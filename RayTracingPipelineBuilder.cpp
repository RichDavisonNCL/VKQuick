/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "RayTracingPipelineBuilder.h"
#include "MemoryManager.h"
#include "Utils.h"

using namespace VKQuick;

RayTracingPipelineBuilder::RayTracingPipelineBuilder(vk::Device device, vk::PhysicalDevice physicalDevice, MemoryManager& memManager) : PipelineBuilderBase(device), m_memoryManager(memManager){
	m_physicalDevice = physicalDevice;
}

RayTracingPipelineBuilder::~RayTracingPipelineBuilder() {
}

RayTracingPipelineBuilder& RayTracingPipelineBuilder::WithRecursionDepth(uint32_t count) {
	m_pipelineCreate.maxPipelineRayRecursionDepth = count;
	return *this;
}

RayTracingPipelineBuilder& RayTracingPipelineBuilder::WithRayGenGroup(uint32_t shaderIndex) {
	vk::RayTracingShaderGroupCreateInfoKHR groupCreateInfo;
	groupCreateInfo.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
	groupCreateInfo.generalShader = shaderIndex;
	m_genGroups.push_back(groupCreateInfo);
	return *this;
}

RayTracingPipelineBuilder& RayTracingPipelineBuilder::WithMissGroup(uint32_t shaderIndex) {
	vk::RayTracingShaderGroupCreateInfoKHR groupCreateInfo;
	groupCreateInfo.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
	groupCreateInfo.generalShader = shaderIndex;
	m_missGroups.push_back(groupCreateInfo);
	return *this;
}

RayTracingPipelineBuilder& RayTracingPipelineBuilder::WithTriangleHitGroup(uint32_t closestHit, uint32_t anyHit) {
	vk::RayTracingShaderGroupCreateInfoKHR groupCreateInfo;

	groupCreateInfo.type				= vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
	groupCreateInfo.generalShader		= VK_SHADER_UNUSED_KHR;
	groupCreateInfo.intersectionShader	= VK_SHADER_UNUSED_KHR;
	groupCreateInfo.closestHitShader	= closestHit;
	groupCreateInfo.anyHitShader		= anyHit;

	m_hitGroups.push_back(groupCreateInfo);

	return *this;
}

RayTracingPipelineBuilder& RayTracingPipelineBuilder::WithProceduralHitGroup(uint32_t intersection, uint32_t closestHit, uint32_t anyHit){
	vk::RayTracingShaderGroupCreateInfoKHR groupCreateInfo;

	groupCreateInfo.type				= vk::RayTracingShaderGroupTypeKHR::eProceduralHitGroup;
	groupCreateInfo.generalShader		= VK_SHADER_UNUSED_KHR;
	groupCreateInfo.intersectionShader	= intersection;
	groupCreateInfo.closestHitShader	= closestHit;
	groupCreateInfo.anyHitShader		= anyHit;

	m_hitGroups.push_back(groupCreateInfo);

	return *this;
}

RayPipeline RayTracingPipelineBuilder::Build(const std::string& debugName, vk::PipelineCache cache) {
	RayPipeline output;
	
	FillShaderModules(output);
	FillShaderLayouts(output);
	
	m_allGroups.clear();
	m_allGroups.insert(m_allGroups.end(), m_genGroups.begin() , m_genGroups.end());
	m_allGroups.insert(m_allGroups.end(), m_missGroups.begin(), m_missGroups.end());
	m_allGroups.insert(m_allGroups.end(), m_hitGroups.begin() , m_hitGroups.end());

	m_pipelineCreate.groupCount	= m_allGroups.size();
	m_pipelineCreate.pGroups	= m_allGroups.data();

	m_pipelineCreate.setPDynamicState(&m_dynamicCreate);

	output.pipeline = m_sourceDevice.createRayTracingPipelineKHRUnique({}, cache, m_pipelineCreate).value;

	if (!debugName.empty()) {
		SetDebugName(m_sourceDevice, *output.pipeline	, debugName);
		SetDebugName(m_sourceDevice, *output.layout		, debugName);
	}

	CreateSBT(output, debugName);

	return output;
}

void RayTracingPipelineBuilder::FillSBTCounts(const vk::RayTracingPipelineCreateInfoKHR* fromInfo) {
	for (int i = 0; i < fromInfo->groupCount; ++i) {
		if (fromInfo->pGroups[i].type == vk::RayTracingShaderGroupTypeKHR::eGeneral) {
			int shaderType = fromInfo->pGroups[i].generalShader;

			if (fromInfo->pStages[shaderType].stage == vk::ShaderStageFlagBits::eRaygenKHR) {
				handleCounts[BindingTableOrder::RayGen]++;
			}
			else if (fromInfo->pStages[shaderType].stage == vk::ShaderStageFlagBits::eMissKHR) {
				handleCounts[BindingTableOrder::Miss]++;
			}
			else if (fromInfo->pStages[shaderType].stage == vk::ShaderStageFlagBits::eCallableKHR) {
				handleCounts[BindingTableOrder::Call]++;
			}
		}
		else { //Must be a hit group
			handleCounts[BindingTableOrder::Hit]++;
		}
	}
}

uint32_t MakeMultipleOf(uint32_t input, uint32_t multiple) {
	uint32_t count = input / multiple;
	uint32_t r = input % multiple;

	if (r != 0) {
		count += 1;
	}

	return count * multiple;
}

vk::DeviceAddress AlignmentOffset(vk::DeviceAddress input, uint32_t alignment) {
	vk::DeviceAddress remainder = input % alignment;
	return alignment - remainder;
}

void RayTracingPipelineBuilder::CreateSBT(RayPipeline& pipeline, const std::string& debugName) {
	FillSBTCounts(&m_pipelineCreate); //Fills the handleIndices vectors

	uint32_t numShaderGroups = m_pipelineCreate.groupCount;

	vk::PhysicalDeviceRayTracingPipelinePropertiesKHR rayPipelineProperties;
	vk::PhysicalDeviceProperties2 properties;
	properties.pNext = &rayPipelineProperties;
	m_physicalDevice.getProperties2(&properties);

	uint32_t handleSize			= rayPipelineProperties.shaderGroupHandleSize;
	uint32_t alignedHandleSize	= MakeMultipleOf(handleSize, rayPipelineProperties.shaderGroupHandleAlignment);
	uint32_t totalHandleSize	= numShaderGroups * handleSize;

	std::vector<uint8_t> handles(totalHandleSize);
	auto result = m_sourceDevice.getRayTracingShaderGroupHandlesKHR(pipeline, 0, numShaderGroups, totalHandleSize, handles.data());

	uint32_t bufferSize = 0;

	for (int i = 0; i < BindingTableOrder::MAX_SIZE; ++i) {
		pipeline.bindingTable.regions[i].size	= MakeMultipleOf(alignedHandleSize * handleCounts[i], rayPipelineProperties.shaderGroupBaseAlignment);
		pipeline.bindingTable.regions[i].stride = alignedHandleSize;
		bufferSize += pipeline.bindingTable.regions[i].size;
	}
	pipeline.bindingTable.regions[0].stride = pipeline.bindingTable.regions[0].size;

	pipeline.bindingTable.tableBuffer = m_memoryManager.CreateBuffer(
		{
			.size = bufferSize * 20,
			.usage =	vk::BufferUsageFlagBits::eShaderDeviceAddress		|
						vk::BufferUsageFlagBits::eTransferSrc				|
						vk::BufferUsageFlagBits::eShaderDeviceAddressKHR	|
						vk::BufferUsageFlagBits::eShaderBindingTableKHR,
		},
		vk::MemoryPropertyFlagBits::eHostVisible,
		debugName + " SBT Buffer"
	);

	vk::DeviceAddress bufferAddress = pipeline.bindingTable.tableBuffer.GetDeviceAddress();
	vk::DeviceAddress bufferOffset	= AlignmentOffset(bufferAddress , rayPipelineProperties.shaderGroupBaseAlignment);

	bufferAddress += bufferOffset;

	char* dataPtr = (char*)pipeline.bindingTable.tableBuffer.Map();
	dataPtr += bufferOffset;

	int dataOffset = 0;

	int currentHandleIndex = 0;

	for (int i = 0; i < BindingTableOrder::MAX_SIZE; ++i) { //For each group type
		dataOffset += AlignmentOffset(dataOffset, rayPipelineProperties.shaderGroupBaseAlignment);

		pipeline.bindingTable.regions[i].deviceAddress = bufferAddress + dataOffset;

		for (int j = 0; j < handleCounts[i]; ++j) { //For entries in that group
			memcpy(dataPtr + dataOffset, handles.data() + (currentHandleIndex++ * handleSize), handleSize);
			dataOffset += alignedHandleSize;
		}
		dataOffset += pipeline.bindingTable.regions[i].size;
	}

	pipeline.bindingTable.tableBuffer.Unmap();
}
