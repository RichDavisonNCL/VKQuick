/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "DynamicRenderBuilder.h"
#include "Utils.h"

using namespace VKQuick;

DynamicRenderBuilder::DynamicRenderBuilder(vk::CommandBuffer cmdBuffer, bool useOpenGLCoordinates) {
	m_cmdBuffer				= cmdBuffer;
	m_useOpenGLCoordinates	= useOpenGLCoordinates;
	m_renderInfo.setLayerCount(1);
}

DynamicRenderBuilder& DynamicRenderBuilder::WithColourAttachment(vk::RenderingAttachmentInfoKHR const&  info) {
	m_colourAttachments.push_back(info);
	return *this;
}

DynamicRenderBuilder& DynamicRenderBuilder::WithDepthAttachment(vk::RenderingAttachmentInfoKHR const&  info) {
	m_depthAttachment = info;
	return *this;
}

DynamicRenderBuilder& DynamicRenderBuilder::WithStencilAttachment(vk::RenderingAttachmentInfoKHR const& info) {
	m_stencilAttachment = info;
	return *this;
}

DynamicRenderBuilder& DynamicRenderBuilder::WithDepthStenclAttachment(vk::RenderingAttachmentInfoKHR const& info) {
	m_depthAttachment = info;
	m_stencilAttachment = info;

	return *this;
}

DynamicRenderBuilder& DynamicRenderBuilder::WithRenderArea(vk::Rect2D area, bool useAutoViewstate) {
	m_renderInfo.setRenderArea(area);
	m_usingAutoViewstate = useAutoViewstate;
	return *this;
}

DynamicRenderBuilder& DynamicRenderBuilder::WithLayerCount(int count) {
	m_renderInfo.setLayerCount(count);
	return *this;
}

DynamicRenderBuilder& DynamicRenderBuilder::WithRenderingFlags(vk::RenderingFlags flags) {
	m_renderInfo.setFlags(flags);
	return *this;
}

DynamicRenderBuilder& DynamicRenderBuilder::WithViewMask(uint32_t viewMask) {
	m_renderInfo.setViewMask(viewMask);
	return *this;
}

DynamicRenderBuilder& DynamicRenderBuilder::WithRenderInfo(vk::RenderingInfoKHR const& info) {
	m_renderInfo = info;
	return *this;
}

void DynamicRenderBuilder::BeginRendering() {
	m_renderInfo
		.setColorAttachments(m_colourAttachments)
		.setPDepthAttachment(&m_depthAttachment)
		.setPStencilAttachment(&m_stencilAttachment);

	m_cmdBuffer.beginRendering(m_renderInfo);

	if (m_usingAutoViewstate) {
		vk::Extent2D	extent		= m_renderInfo.renderArea.extent;
		vk::Rect2D		scissor		= vk::Rect2D(vk::Offset2D(0, 0), extent);

		vk::Viewport	viewport;
		
		if (m_useOpenGLCoordinates) {
			viewport = vk::Viewport(0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f);
		}
		else {
			viewport = vk::Viewport(0.0f, (float)extent.height, (float)extent.width, -(float)extent.height, 0.0f, 1.0f);
		}
	
		m_cmdBuffer.setViewport(0, 1, &viewport);
		m_cmdBuffer.setScissor( 0, 1, &scissor);
	}
}

ScopedDynamicRendering DynamicRenderBuilder::BeginScopedRendering(const std::string& debugName) {
	BeginRendering();	
	if (!debugName.empty()) {
		VKQuick::BeginDebugArea(m_cmdBuffer,debugName);
	}
	return std::move(ScopedDynamicRendering(m_cmdBuffer, !debugName.empty()));
}

ScopedDynamicRendering::~ScopedDynamicRendering() {
	if (m_cmdBuffer) {
		m_cmdBuffer.endRendering();	
		if (m_endDebugArea) {
			VKQuick::EndDebugArea(m_cmdBuffer);
		}
	}
}