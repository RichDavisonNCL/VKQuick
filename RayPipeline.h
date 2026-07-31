/******************************************************************************
This file is part of the VKQuick library

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once

namespace VKQuick {
	namespace BindingTableOrder {
		enum Type : uint32_t {
			RayGen,
			Miss,
			Hit,
			Call,
			MAX_SIZE
		};
	};

	struct ShaderBindingTable {
		Buffer tableBuffer;
		vk::StridedDeviceAddressRegionKHR regions[BindingTableOrder::MAX_SIZE];
	};

	struct RayPipeline : public Pipeline {
	//public:
		ShaderBindingTable bindingTable;
	};
}