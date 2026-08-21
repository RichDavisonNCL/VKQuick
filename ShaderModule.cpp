/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "ShaderModule.h"
#include "Utils.h"

extern "C" {
#include "Spirv-reflect/Spirv_reflect.h"
}

using std::ifstream;

using namespace VKQuick;

std::vector<char>	ReadBinaryFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);

	std::filesystem::path inputFilePath{ filename };

	if (!std::filesystem::exists(inputFilePath)) {
		std::cout << __FUNCTION__ << ": Failed to read binary file " << filename << "\n";
		return {};
	}

	uintmax_t length = std::filesystem::file_size(inputFilePath);
	std::vector<char> data(length);
	file.read(data.data(), length);

	return data;
}


ShaderModule::ShaderModule(const std::string& filename, vk::ShaderStageFlagBits stage, vk::Device device)	{
	std::vector<char> data = ReadBinaryFile(filename);

	if (data.size() > 0) {
		m_shaderModule = device.createShaderModuleUnique(
			{
				.flags		= {},
				.codeSize	= data.size(),
				.pCode		= (const uint32_t*)data.data()	
			}
		);
		AddReflectionData(data, stage);

		VKQuick::SetDebugName(device, *m_shaderModule, filename);
	}
	else {
		std::cout << __FUNCTION__ << ": Problem loading shader file " << filename << "!\n";
		assert(data.size() > 0);
	}
	m_shaderStage	= stage;
	m_fileName		= filename;
}

void ShaderModule::CombineLayoutBindings(std::vector<std::vector<vk::DescriptorSetLayoutBinding>>& inoutBindings, vk::ShaderStageFlags layoutStage) const {
	const int numSets = std::max(inoutBindings.size(), m_allLayoutsBindings.size());
	inoutBindings.resize(numSets);

	if (!layoutStage) {
		layoutStage = m_shaderStage;
	}

	for (int i = 0; i < m_allLayoutsBindings.size(); ++i) {
		std::vector<vk::DescriptorSetLayoutBinding>& outSet		= inoutBindings[i];
		const std::vector<vk::DescriptorSetLayoutBinding>& baseSet	= m_allLayoutsBindings[i];

		const int numBindings = std::max(outSet.size(), baseSet.size());
		outSet.resize(numBindings);

		for (int j = 0; j < baseSet.size(); ++j) {
			if (baseSet[j].stageFlags != vk::ShaderStageFlags()) {
				//Check that something hasn't gone wrong with the binding combo!
				if (baseSet[j].descriptorType != outSet[j].descriptorType) {

				}
				if (baseSet[j].descriptorCount != outSet[j].descriptorCount) {

				}
				outSet[j].binding			= j;
				outSet[j].descriptorCount	= baseSet[j].descriptorCount;
				outSet[j].descriptorType	= baseSet[j].descriptorType;			
				
				outSet[j].stageFlags		|= layoutStage; //Combine sets across shader stages
			}
		}
	}
}

void ShaderModule::CombinePushConstantRanges(std::vector< vk::PushConstantRange>& inoutRanges) const {
	for (int i = 0; i < m_pushConstants.size(); ++i) {
		bool found = false;
		for (int j = 0; j < inoutRanges.size(); ++j) {
			if (m_pushConstants[i].offset == inoutRanges[j].offset &&
				m_pushConstants[i].size == inoutRanges[j].size) {
				inoutRanges[j].stageFlags |= m_shaderStage;
				found = true;
				break;
			}
		}
		if (!found) {
			inoutRanges.push_back(m_pushConstants[i]);
		}
	}
}

void ShaderModule::AddReflectionData(const std::vector<char>& data, vk::ShaderStageFlags stage) {
	SpvReflectShaderModule module;
	SpvReflectResult result = spvReflectCreateShaderModule(data.size(), data.data(), &module);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	uint32_t descriptorCount = 0;
	result = spvReflectEnumerateDescriptorSets(&module, &descriptorCount, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	std::vector<SpvReflectDescriptorSet*> descriptorSetLayouts(descriptorCount);
	result = spvReflectEnumerateDescriptorSets(&module, &descriptorCount, descriptorSetLayouts.data());
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	for (auto& set : descriptorSetLayouts) {
		if (set->set >= m_allLayoutsBindings.size()) {
			m_allLayoutsBindings.resize(set->set + 1);
		}
		std::vector<vk::DescriptorSetLayoutBinding>& setLayout = m_allLayoutsBindings[set->set];

		for (int i = 0; i < set->binding_count; ++i) {
			SpvReflectDescriptorBinding* binding = set->bindings[i];

			uint32_t index = binding->binding;
			uint32_t count = binding->count;

			if (index >= setLayout.size()) {
				setLayout.resize(index + 1);
			}

			if (setLayout[index].stageFlags != vk::ShaderStageFlags()) {
				//Check that something hasn't gone wrong with the binding combo!
				//if (setLayout[index].descriptorType != (vk::DescriptorType)binding->descriptor_type) {

				//}
				//if (setLayout[index].descriptorCount != binding->count) {

				//}
			}
			setLayout[index].binding			= index;
			setLayout[index].descriptorCount	= binding->count;
			setLayout[index].descriptorType		= (vk::DescriptorType)binding->descriptor_type;

			setLayout[index].stageFlags |= stage; //Combine sets across shader stages
		}
	}

	uint32_t pushConstantCount = 0;
	result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	std::vector<SpvReflectBlockVariable*> pushConstantLayouts(pushConstantCount);
	result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, pushConstantLayouts.data());
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	for (auto& constant : pushConstantLayouts) {
		for (int i = 0; i < constant->member_count; ++i) {
			auto& member = constant->members[i];
			//Check to see if this one was loaded 
			bool found = false;
			for (int i = 0; i < m_pushConstants.size(); ++i) {
				if (m_pushConstants[i].offset == member.offset &&
					m_pushConstants[i].size == member.size) {
					m_pushConstants[i].stageFlags |= stage;
					found = true;
					break;
				}
			}
			if (!found) {
				vk::PushConstantRange range;
				range.offset = constant->offset;
				range.size = constant->size;
				range.stageFlags = stage;
				m_pushConstants.push_back(range);
			}
		}
	}
	spvReflectDestroyShaderModule(&module);
}