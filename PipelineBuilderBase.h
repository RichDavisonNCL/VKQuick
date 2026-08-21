/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Pipeline.h"
#include "ShaderModule.h"
#include "Utils.h"

namespace VKQuick {
	struct VulkanVertexSpecification;

	template <class T, class P>
	class PipelineBuilderBase	{
	public:

		T& WithLayout(vk::PipelineLayout pipeLayout) {
			m_layout = pipeLayout;
			m_pipelineCreate.setLayout(pipeLayout);
			return (T&)*this;
		}

		T& WithDescriptorSetLayout(uint32_t setIndex, const vk::UniqueDescriptorSetLayout& m_layout) {
			return WithDescriptorSetLayout(setIndex, *m_layout);
		}

		T& WithDescriptorSetLayout(uint32_t setIndex, vk::DescriptorSetLayout m_layout) {
			assert(setIndex < 32);
			if (setIndex >= m_userLayouts.size()) {
				while (m_userLayouts.size() <= setIndex) {
					m_userLayouts.push_back(VK_NULL_HANDLE);
				}
			}
			m_userLayouts[setIndex] = m_layout;
			return (T&)*this;
		}

		T& WithDescriptorSetLayoutCreationFlags(uint32_t setIndex, vk::DescriptorSetLayoutCreateFlags flags) {
			m_userLayoutCreationFlags[setIndex] = flags;
			return (T&)*this;
		}

		T& WithCreationFlags(vk::PipelineCreateFlagBits flags) {
			m_pipelineCreate.flags |= flags;
			return (T&)*this;
		}

		T& WithCreationFlags(vk::PipelineCreateFlagBits2 flags) {
			m_pipelineCreateBits |= flags;
			return (T&)*this;
		}

		P& GetCreateInfo() {
			return m_pipelineCreate;
		}

		T& WithDescriptorSetAndBindingMappingInfo(const vk::ShaderDescriptorSetAndBindingMappingInfoEXT& info) {
			m_heapBindingInfo = info;
			return (T&)*this;
		}

		T& WithMinimisedSetLayoutStages() {
			minimiseLayoutStages = true;
			return (T&)*this;
		}

		T& WithShaderBinary(const std::string& filename, vk::ShaderStageFlagBits stage,const std::string& entrypoint = "main") {
			std::string searchName = VKQuick::shaderFolderRoot + filename;

			//for (auto& i : m_loadedShaderModules) {
			//	if (i->m_fileName == searchName) {
			//		//We've already loaded this binary once!

			//	}
			//}

			m_loadedShaderModules.push_back(std::make_unique<ShaderModule>(searchName, m_sourceDevice));

			m_usedModules.push_back(m_loadedShaderModules.back().get());
			m_moduleEntryPoints.push_back(entrypoint);
			m_moduleShaderStages.push_back(stage);
			return (T&)*this;
		}

		T& WithShaderModule(const ShaderModule& module, vk::ShaderStageFlagBits stage, const std::string& entrypoint = "main") {
			m_usedModules.push_back(&module);
			m_moduleEntryPoints.push_back(entrypoint);
			m_moduleShaderStages.push_back(stage);
			return (T&)*this;
		}

	protected:
		PipelineBuilderBase(vk::Device device) {
			m_sourceDevice = device;
		}
		~PipelineBuilderBase() = default;

		void FillShaderModules(Pipeline& output) {
			for (int i = 0; i < m_usedModules.size(); ++i) {
				vk::PipelineShaderStageCreateInfo stageInfo;

				stageInfo.pName = m_moduleEntryPoints[i].c_str();
				stageInfo.stage = m_moduleShaderStages[i];
				stageInfo.module = *m_usedModules[i]->m_shaderModule;

				m_shaderStages.push_back(stageInfo);
			}
			m_pipelineCreate.setStageCount(m_shaderStages.size());
			m_pipelineCreate.setPStages(m_shaderStages.data());
		}

		void FillShaderLayouts(Pipeline& output) {
			if (m_externalLayout) {
				m_pipelineCreate.setLayout(m_externalLayout);
			}
			else {
				for (int i = 0; i < m_usedModules.size(); ++i) {

					vk::ShaderStageFlags stageFlags = minimiseLayoutStages ? m_moduleShaderStages[i] : vk::ShaderStageFlagBits::eAll;
					m_usedModules[i]->CombineLayoutBindings(output.m_allLayoutsBindings, stageFlags);
					m_usedModules[i]->CombinePushConstantRanges(output.m_pushConstants, stageFlags);
				}
				FinaliseLayout(output);
			}
		}

		void FinaliseLayout(Pipeline& output) {
			if (m_pipelineCreateBits & vk::PipelineCreateFlagBits2::eDescriptorHeapEXT) {
				return;
			}

			output.m_allLayouts.resize(output.m_allLayoutsBindings.size());
			for (int i = 0; i < output.m_allLayoutsBindings.size(); ++i) {
				if (i < m_userLayouts.size() && m_userLayouts[i]) {
					output.m_allLayouts[i] = m_userLayouts[i];
				}
				else {
					vk::DescriptorSetLayoutCreateInfo createInfo;
					createInfo.setBindings(output.m_allLayoutsBindings[i]);

					auto userFlags = m_userLayoutCreationFlags.find(i);

					if (userFlags != m_userLayoutCreationFlags.end()) {
						createInfo.flags |= userFlags->second;
					}
					output.m_createdLayouts.push_back(m_sourceDevice.createDescriptorSetLayoutUnique(createInfo));
					output.m_allLayouts[i] = output.m_createdLayouts.back().get();
				}
			}
			vk::PipelineLayoutCreateInfo pipeLayoutCreate = vk::PipelineLayoutCreateInfo();
			pipeLayoutCreate.setSetLayouts(output.m_allLayouts);
			pipeLayoutCreate.setPushConstantRanges(output.m_pushConstants);
			output.layout = m_sourceDevice.createPipelineLayoutUnique(pipeLayoutCreate);
			m_pipelineCreate.setLayout(*output.layout);
		}

		static void SetShaderFolderRoot(const std::string& root) {
			shaderFolderRoot = root;
		}

	protected:
		P m_pipelineCreate;
		vk::PipelineLayout	m_layout;
		vk::Device			m_sourceDevice;

		bool minimiseLayoutStages = false;

		vk::PipelineCreateFlags2				m_pipelineCreateBits;

		vk::ShaderDescriptorSetAndBindingMappingInfoEXT m_heapBindingInfo;

		vk::PipelineLayout						m_externalLayout;

		std::map<uint32_t, vk::DescriptorSetLayoutCreateFlags> m_userLayoutCreationFlags;

		std::vector< vk::DescriptorSetLayout>	m_userLayouts;

		std::vector<vk::PipelineShaderStageCreateInfo>	m_shaderStages;
		std::vector<const ShaderModule*>			m_usedModules;
		std::vector<UniqueShaderModule>			m_loadedShaderModules;
		std::vector<std::string>						m_moduleEntryPoints;
		std::vector< vk::ShaderStageFlagBits >			m_moduleShaderStages;
	};
}