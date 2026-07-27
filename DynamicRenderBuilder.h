/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once

namespace VKQuick {
	class Instance;

	class ScopedDynamicRendering {
	public:
		friend class DynamicRenderBuilder;

		~ScopedDynamicRendering();
	protected:
		ScopedDynamicRendering(vk::CommandBuffer cmdBuffer, bool endDebugArea) {
			m_cmdBuffer		= cmdBuffer;
			m_endDebugArea	= endDebugArea;
		}

		ScopedDynamicRendering(ScopedDynamicRendering&& other) noexcept {
			m_cmdBuffer		= other.m_cmdBuffer;
			m_endDebugArea	= other.m_endDebugArea;
			other.m_cmdBuffer = VK_NULL_HANDLE;
		}

		vk::CommandBuffer m_cmdBuffer;
		bool m_endDebugArea;
	};


	/*
	DynamicRenderBuilder: This helper class provides a means to set up
	the vk::RenderingInfoKHR struct and call beginRendering.

	Why have this?

	In this case, the builder pattern keeps things a little clearer (all your
	variables fall out of scope once the rendering begins and can't be used
	accidentally), while also allowing us to automatically handle some of the 
	awkward cases that surround the coordinate system and dynamic state - this
	way, we can set up the dynamic viewport / render area, AND hide the 
	OpenGL vs everything else screen coordinate system fun.

	BeginScopedRendering returns a small struct that will automatically
	end the dynamic rendering pass once it falls out of scope. I use this
	by putting the rendering code just in a scope - remember, you can just
	put braces anywhere in this crazy language!

	*/
	class DynamicRenderBuilder	{
	public:
		DynamicRenderBuilder(vk::CommandBuffer cmdBuffer, bool useOpenGLCoordinates = false);
		~DynamicRenderBuilder() = default;

		DynamicRenderBuilder& WithColourAttachment(vk::RenderingAttachmentInfoKHR const & info);
		DynamicRenderBuilder& WithDepthAttachment(vk::RenderingAttachmentInfoKHR const& info);
		DynamicRenderBuilder& WithStencilAttachment(vk::RenderingAttachmentInfoKHR const& info);
		DynamicRenderBuilder& WithDepthStenclAttachment(vk::RenderingAttachmentInfoKHR const& info);

		DynamicRenderBuilder& WithRenderArea(vk::Rect2D area, bool useAutoViewstate = true);

		DynamicRenderBuilder& WithRenderingFlags(vk::RenderingFlags flags);
		DynamicRenderBuilder& WithViewMask(uint32_t viewMask);
		DynamicRenderBuilder& WithLayerCount(int count);
		DynamicRenderBuilder& WithRenderInfo(vk::RenderingInfoKHR const & info);

		void BeginRendering();

		[[nodiscard]]
		ScopedDynamicRendering BeginScopedRendering(const std::string& debugName = "");

	protected:
		vk::RenderingInfoKHR	m_renderInfo;
		vk::CommandBuffer		m_cmdBuffer;
		bool					m_useOpenGLCoordinates;
		std::vector< vk::RenderingAttachmentInfoKHR > m_colourAttachments;
		vk::RenderingAttachmentInfoKHR m_depthAttachment;
		vk::RenderingAttachmentInfoKHR m_stencilAttachment;
		bool		m_usingAutoViewstate	= true;
	};
}