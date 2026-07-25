/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MemoryManager.h"
#include "Pipeline.h"

using std::string;

namespace VKQuick {
	namespace CommandType {
		enum Type : uint32_t {
			Graphics,
			AsyncCompute,
			Copy,
			Present,
			MAX_COMMAND_TYPES
		};
	};

	struct FrameContext {
		vk::Device			device;
		vk::DescriptorPool  descriptorPool;
		vk::CommandBuffer	cmdBuffer;		
		
		vk::CommandPool		commandPools[CommandType::Type::MAX_COMMAND_TYPES];
		vk::Queue			queues[CommandType::Type::MAX_COMMAND_TYPES];
		uint32_t			queueFamilies[CommandType::Type::MAX_COMMAND_TYPES];

		vk::Image			colourImage;
		vk::ImageView		colourView;
		vk::Format			colourFormat;

		vk::Image			depthImage;
		vk::ImageView		depthView;
		vk::Format			depthFormat;

		vk::Viewport		viewport;
		vk::Rect2D			screenRect;

		uint32_t			frameID = 0;
		uint32_t			cycleID = 0;
		uint64_t			waitID	= 0;
	};

	struct AcquireState {
		vk::Semaphore		acquireSempaphore;
		vk::Fence			acquireFence;
	};

	struct ChainState {
		vk::Framebuffer		frameBuffer;
		vk::Image			colourImage;
		vk::ImageView		colourView;
		vk::Format			colourFormat;

		vk::Semaphore		presentSempaphore;
		vk::Fence			presentFence;
	};

	struct VKQuickInitialisation {
		vk::Format			depthStencilFormat	= vk::Format::eD32SfloatS8Uint;
		vk::PresentModeKHR  idealPresentMode	= vk::PresentModeKHR::eFifo;

		vk::PhysicalDeviceType idealGPU			= vk::PhysicalDeviceType::eDiscreteGpu;


		vk::DescriptorPoolCreateFlags	defaultDescriptorPoolFlags = {};

		uint32_t	defaultDescriptorPoolBufferCount	= 128;
		uint32_t	defaultDescriptorPoolImageCount		= 128;
		uint32_t	defaultDescriptorPoolSamplerCount	= 32;
		uint32_t	defaultDescriptorPoolAccelerationStructureCount	= 0;

		uint32_t	majorVersion	= 1;
		uint32_t	minorVersion	= 1;

		uint32_t	framesInFlight	= 3;

		bool		useOpenGLCoordinates		= false;

		bool		useHDRSurface				= false;

		std::string applicationName = "VKQuick Application";

		std::string shaderRoot = "";

		std::vector<void*>			features;

		std::vector<const char*>	instanceExtensions;
		std::vector<const char*>	instanceLayers;

		std::vector<const char*>	deviceExtensions;	
		std::vector<const char*>	deviceLayers;

		uint32_t	initialWidth	= 0;
		uint32_t	initialHeight	= 0;

#ifdef _WIN32
		HWND		win32Handle;
		HINSTANCE	win32Instance;
#endif
	};

	class Instance {
	public:
		Instance(const VKQuickInitialisation& vkInit);
		~Instance();

		bool HasInitialised() const { return m_device; }

		vk::Device GetDevice() const {
			return m_device;
		}

		vk::PhysicalDevice GetPhysicalDevice() const {
			return m_physicalDevice;
		}

		vk::Instance	GetVulkanInstance() const {
			return m_instance;
		}

		FrameContext const& GetFrameContext() const {
			return m_frameContexts[m_currentFrameContextID];
		}

		bool SupportsAsyncCompute() const;
		bool SupportsAsyncCopy()	const;
		bool SupportsAsyncPresent() const;

		bool MemoryTypeFromPhysicalDeviceProps(vk::MemoryPropertyFlags requirements, uint32_t type, uint32_t& index);

		void	BeginDefaultRenderPass(vk::CommandBuffer cmds);
		void	BeginRenderToScreen(vk::CommandBuffer  cmds);

		void BeginFrame();
		void EndFrame();
		void SwapBuffers();

		void OnWindowResize(int w, int h);
		void OnWindowMinimise(bool isMinimised);
	protected:
		void	InitDefaultRenderPass();
		void	InitDefaultDescriptorPool();

	private: 
		void	InitCommandPools();
		bool	InitInstance();
		bool	InitPhysicalDevice();
		bool	InitGPUDevice();
		bool	InitSurface();
		void	InitBufferChain(vk::CommandBuffer  m_cmdBuffer);

		void	InitFrameStates(uint32_t framesInFlight);

		void	CreateDepthBufer(uint32_t width, uint32_t height);

		static VkBool32 DebugCallbackFunction(
			vk::DebugUtilsMessageSeverityFlagBitsEXT			messageSeverity,
			vk::DebugUtilsMessageTypeFlagsEXT					messageTypes,
			const vk::DebugUtilsMessengerCallbackDataEXT*		pCallbackData,
			void*												pUserData);

		bool	InitDeviceQueueIndices();
	private:
		VKQuickInitialisation		m_vkInit;

		vk::Instance				m_instance;			//API Instance
		vk::PhysicalDevice			m_physicalDevice;	//GPU in use

		vk::DescriptorPool			m_defaultDescriptorPool;	//descriptor sets come from here!

		vk::CommandPool				m_commandPools[CommandType::Type::MAX_COMMAND_TYPES];
		vk::Queue					m_queues[CommandType::Type::MAX_COMMAND_TYPES];
		uint32_t					m_queueFamilies[CommandType::Type::MAX_COMMAND_TYPES];

		vk::PhysicalDeviceProperties		m_deviceProperties;
		vk::PhysicalDeviceMemoryProperties	m_deviceMemoryProperties;
		vk::DebugUtilsMessengerEXT			m_debugMessenger;

		vk::PipelineCache			m_pipelineCache;
		vk::Device					m_device;		//Device handle	

		vk::SwapchainKHR			m_swapChain;
		vk::SurfaceKHR				m_surface;
		vk::Format					m_surfaceFormat;
		vk::ColorSpaceKHR			m_surfaceSpace;
	
		vk::Image					m_depthImage;
		vk::ImageView				m_depthView;
		vk::DeviceMemory			m_depthMemory;

		vk::Semaphore				m_flightSempaphore;

		vk::Viewport				m_defaultViewport;
		vk::Rect2D					m_defaultScissor;
		vk::Rect2D					m_defaultScreenRect;
		vk::RenderPass				m_defaultRenderPass;
		vk::RenderPassBeginInfo		m_defaultBeginInfo;

		std::vector<FrameContext>	m_frameContexts;
		std::vector<ChainState>		m_swapStates;
		std::vector<AcquireState>	m_acquireStates;

		FrameContext*				m_currentFrameContext	= nullptr;
		ChainState*					m_currentSwapState		= nullptr;
		AcquireState*				m_currentAcquireState	= nullptr;

		uint32_t					m_currentFrameContextID	= 0; //Frame context for this frame
		uint32_t					m_currentSwap			= 0; //To index our swapchain 
		uint64_t					m_globalFrameID			= 0;

		vk::Extent2D				m_windowSize;

		bool						m_windowMinimised		= false;
	};
}