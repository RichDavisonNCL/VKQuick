/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "Utils.h"
#include "Texture.h"
#include "Buffer.h"

using namespace VKQuick;

vk::detail::DynamicLoader VKQuick::dynamicLoader;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

void VKQuick::BeginDebugArea(vk::CommandBuffer b, const std::string& name) {
	vk::DebugUtilsLabelEXT labelInfo;
	labelInfo.pLabelName = name.c_str();
	b.beginDebugUtilsLabelEXT(labelInfo);
}

void VKQuick::EndDebugArea(vk::CommandBuffer b) {
	b.endDebugUtilsLabelEXT();
}

vk::AccessFlags VKQuick::DefaultAccessFlags(vk::ImageLayout forLayout) {
	if (forLayout == vk::ImageLayout::eTransferDstOptimal) {
		return vk::AccessFlagBits::eTransferWrite;
	}
	else if (forLayout == vk::ImageLayout::eTransferSrcOptimal) {
		return vk::AccessFlagBits::eTransferRead;
	}
	else if (forLayout == vk::ImageLayout::eColorAttachmentOptimal) {
		return vk::AccessFlagBits::eColorAttachmentWrite;
	}
	else if (forLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
		return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}
	else if (forLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		return vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eInputAttachmentRead; //added last bit?!?
	}
	else if (forLayout == vk::ImageLayout::eGeneral) {
		return vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eInputAttachmentRead;
	}
	return vk::AccessFlagBits::eNone;
}

vk::AccessFlags2 VKQuick::DefaultAccessFlags2(vk::ImageLayout forLayout) {
	if (forLayout == vk::ImageLayout::eTransferDstOptimal) {
		return vk::AccessFlagBits2::eTransferWrite;
	}
	else if (forLayout == vk::ImageLayout::eTransferSrcOptimal) {
		return vk::AccessFlagBits2::eTransferRead;
	}
	else if (forLayout == vk::ImageLayout::eColorAttachmentOptimal) {
		return vk::AccessFlagBits2::eColorAttachmentWrite;
	}
	else if (forLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
		return vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
	}
	else if (forLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		return vk::AccessFlagBits2::eShaderRead | vk::AccessFlagBits2::eInputAttachmentRead; //added last bit?!?
	}
	return vk::AccessFlagBits2::eNone;
}

void VKQuick::ImageTransitionBarrier(vk::CommandBuffer  buffer, vk::Image i, vk::ImageMemoryBarrier2 barrier) {
	barrier.image = i;
	buffer.pipelineBarrier2({
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier
	});
}

void	VKQuick::ImageTransitionBarrier(vk::CommandBuffer  cmdBuffer, vk::Image image, 
	vk::ImageLayout oldLayout, vk::ImageLayout newLayout, 
	vk::ImageAspectFlags aspect, 
	vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage, 
	uint32_t mipLevel, uint32_t mipCount, uint32_t layer, uint32_t layerCount) {

	vk::ImageMemoryBarrier2 memoryBarrier2{
		.srcStageMask	= srcStage,	
		.dstStageMask	= dstStage,
		.dstAccessMask	= DefaultAccessFlags2(newLayout),
		.oldLayout		= oldLayout,
		.newLayout		= newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image			= image,
		.subresourceRange{
			.aspectMask		= aspect,
			.baseMipLevel	= mipLevel,
			.levelCount		= mipCount,
			.baseArrayLayer = layer,
			.layerCount		= layerCount,
		}
	};	
	cmdBuffer.pipelineBarrier2({
		.imageMemoryBarrierCount	= 1,
		.pImageMemoryBarriers		= &memoryBarrier2
	});
}

void VKQuick::TransitionUndefinedToColour(vk::CommandBuffer  buffer, vk::Image t) {
	ImageTransitionBarrier(buffer, t,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageAspectFlagBits::eColor, vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput);
}

void VKQuick::TransitionColourToPresent(vk::CommandBuffer  buffer, vk::Image t) {
	ImageTransitionBarrier(buffer, t,
		vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::ImageAspectFlagBits::eColor,
		vk::PipelineStageFlagBits2::eAllCommands, vk::PipelineStageFlagBits2::eBottomOfPipe);
}

void VKQuick::TransitionColourToSampler(vk::CommandBuffer  buffer, vk::Image t) {
	ImageTransitionBarrier(buffer, t,
		vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eFragmentShader);
}

void VKQuick::TransitionSamplerToColour(vk::CommandBuffer  buffer, vk::Image t) {
	ImageTransitionBarrier(buffer, t,
		vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor,
		vk::PipelineStageFlagBits2::eFragmentShader, vk::PipelineStageFlagBits2::eColorAttachmentOutput);
}

void VKQuick::TransitionDepthToSampler(vk::CommandBuffer  buffer, vk::Image t, bool doStencil) {
	vk::ImageAspectFlags flags = doStencil ? vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil : vk::ImageAspectFlagBits::eDepth;

	ImageTransitionBarrier(buffer, t,
		vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eDepthStencilReadOnlyOptimal, flags,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests, vk::PipelineStageFlagBits2::eFragmentShader);
}

void VKQuick::TransitionSamplerToDepth(vk::CommandBuffer  buffer, vk::Image t, bool doStencil) {
	vk::ImageAspectFlags flags = doStencil ? vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil : vk::ImageAspectFlagBits::eDepth;

	ImageTransitionBarrier(buffer, t,
		vk::ImageLayout::eDepthStencilReadOnlyOptimal, vk::ImageLayout::eDepthStencilAttachmentOptimal, flags,
		vk::PipelineStageFlagBits2::eFragmentShader, vk::PipelineStageFlagBits2::eEarlyFragmentTests);
}

bool VKQuick::MessageAssert(bool condition, const char* msg) {
	if (!condition) {
		std::cerr << msg << "\n";
	}
	return condition;
}

void	VKQuick::CmdBufferResetBegin(vk::CommandBuffer  buffer) {
	buffer.reset();
	buffer.begin(vk::CommandBufferBeginInfo());
}

void	VKQuick::CmdBufferSubmit(const CmdSubmission& submission) {
	if (!submission.buffer) {
		std::cout << __FUNCTION__ << " Submitting invalid buffer?\n";
		return;
	}
	submission.buffer.end();

	uint32_t signalCount	= submission.signalSemaphore	? 1 : 0;
	uint32_t waitCount		= submission.waitSemaphore		? 1 : 0;

	vk::TimelineSemaphoreSubmitInfo tlSubmit{
		.waitSemaphoreValueCount	= waitCount,
		.pWaitSemaphoreValues		= &(submission.waitValue),
		.signalSemaphoreValueCount	= signalCount,
		.pSignalSemaphoreValues		= &(submission.signalValue),
	};

	vk::SubmitInfo submitInfo = {
		.pNext = &tlSubmit,

		.waitSemaphoreCount = waitCount,
		.pWaitSemaphores	= &submission.waitSemaphore,
		.pWaitDstStageMask	= &submission.waitStage,

		.commandBufferCount = 1,
		.pCommandBuffers	= &submission.buffer,

		.signalSemaphoreCount	= signalCount,
		.pSignalSemaphores		= &submission.signalSemaphore
	};

	if (submission.wait) {
		vk::Fence waitFence = submission.device.createFence({});

		submission.queue.submit(submitInfo, waitFence);

		if (submission.device.waitForFences(1, &waitFence, true, UINT64_MAX) != vk::Result::eSuccess) {
			std::cout << __FUNCTION__ << " Device queue submission taking too long?\n";
		};

		submission.device.destroyFence(waitFence);
	}
	else {
		submission.queue.submit(submitInfo, submission.fence);
	}	
}

void  VKQuick::UploadTextureData(vk::CommandBuffer  buffer, vk::Buffer tempBuffer, vk::Image image, vk::ImageLayout currentLyout, vk::ImageLayout endLayout, vk::BufferImageCopy copyInfo) {
	ImageTransitionBarrier(buffer, image, 
		currentLyout, 
		vk::ImageLayout::eTransferDstOptimal, 
		copyInfo.imageSubresource.aspectMask, 
		vk::PipelineStageFlagBits2::eHost, 
		vk::PipelineStageFlagBits2::eTransfer, 
		0, 1);

	buffer.copyBufferToImage(tempBuffer, image, vk::ImageLayout::eTransferDstOptimal, copyInfo);

	ImageTransitionBarrier(buffer, image, 
		vk::ImageLayout::eTransferDstOptimal, 
		endLayout, 
		copyInfo.imageSubresource.aspectMask, 
		vk::PipelineStageFlagBits2::eTransfer, 
		vk::PipelineStageFlagBits2::eAllCommands,
		0, 1);
}

bool  VKQuick::FormatIsDepth(vk::Format format) {
	switch (format) {
	case vk::Format::eD16Unorm:
	case vk::Format::eD32Sfloat:
	case vk::Format::eX8D24UnormPack32:
		return true;
	}
	return false;
}

bool  VKQuick::FormatIsDepthStencil(vk::Format format) {
	switch (format) {
	case vk::Format::eD16UnormS8Uint:
	case vk::Format::eD24UnormS8Uint:
	case vk::Format::eD32SfloatS8Uint:
		return true;
	}
	return false;
}

#define USEDEVICESIGNATURE vk::Device device,
#define USEDEVICEPARAMETER device,
#define USEDEVICENAMESPACE VKQuick::
#include "DeviceFuncs.cpp"
#undef USEDEVICESIGNATURE
#undef USEDEVICEPARAMETER
#undef USEDEVICENAMESPACE