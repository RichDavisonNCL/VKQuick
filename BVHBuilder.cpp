///******************************************************************************
//This file is part of the VKQuick library
//
//Author:Rich Davison
//Contact:richgdavison@gmail.com
//License: MIT (see LICENSE file at the top of the source tree)
//*//////////////////////////////////////////////////////////////////////////////
#include "BVHBuilder.h"
#include "Utils.h"

using namespace VKQuick;

BVHBuilder::BVHBuilder(vk::Device inDevice, VKQuick::MemoryManager& inAllocator) : m_memoryManager(inAllocator){
	m_device		= inDevice;
}

BVHBuilder::~BVHBuilder() {
}

BVHBuilder& BVHBuilder::WithCommandQueue(vk::Queue inQueue) {
	m_queue = inQueue;
	return *this;
}

BVHBuilder& BVHBuilder::WithCommandPool(vk::CommandPool inPool) {
	m_pool = inPool;
	return *this;
}

BVHBuilder& BVHBuilder::WithFlags(vk::BuildAccelerationStructureFlagsKHR flags) {
	m_flags = flags;
	return *this;
}

BVHBuilder& BVHBuilder::WithObject(BVHInput input) {
	auto savedMesh = m_uniqueMeshes.find(input.mesh);

	uint32_t meshID = 0;

	if (savedMesh == m_uniqueMeshes.end()) {
		m_meshes.push_back(input.mesh);
		meshID = m_meshes.size() - 1;
		m_uniqueMeshes.insert({ input.mesh, meshID});
	}
	else {
		meshID = savedMesh->second;
	}

	BVHEntry entry	= static_cast<BVHEntry>(input);
	entry.meshID	= meshID;

	m_entries.push_back(entry);

	return *this;
}


vk::UniqueAccelerationStructureKHR BVHBuilder::Build(const std::string& debugName) {
	BuildBLAS(m_flags);
	BuildTLAS(m_flags);

	if (!debugName.empty()) {
		SetDebugName(m_device, *m_tlas, debugName);
	}

	return std::move(m_tlas);
}

void BVHBuilder::BuildBLAS(vk::BuildAccelerationStructureFlagsKHR inFlags) {
	//We need to first create the BLAS entries for the unique meshes
	for (const auto& i : m_meshes) {
		size_t posIndex = 0;	
		i->GetAttributeIndex(VKQuick::AttributeType::Position, posIndex);

		const VKQuick::Buffer& buffer = i->GetBuffer();

		AttributeData	attributeData;
		IndexData		indexData;

		bool hasPositions	= i->GeAttributeData(posIndex, attributeData);
		bool hasIndices		= i->GetIndexData(indexData);

		vk::AccelerationStructureGeometryTrianglesDataKHR triData;
		triData.vertexFormat = attributeData.format;
		triData.vertexData.deviceAddress = buffer.GetDeviceAddress() + attributeData.offset;
		triData.vertexStride = attributeData.stride;

		if (hasIndices) {
			triData.indexType = indexData.type;
			triData.indexData.deviceAddress = buffer.GetDeviceAddress() + indexData.offset;
		}

		triData.maxVertex = i->GetVertexCount();

		m_blasBuildInfo.resize(m_blasBuildInfo.size() + 1);

		BLASEntry& blasEntry = m_blasBuildInfo.back();


		const std::vector<MeshRange>& ranges = i->GetRanges();

		size_t subMeshCount = ranges.size();

		blasEntry.buildInfo.geometryCount	= subMeshCount;

		blasEntry.geometries.resize(subMeshCount);
		blasEntry.ranges.resize(subMeshCount);
		blasEntry.maxPrims.resize(subMeshCount);

		for (int j = 0; j < subMeshCount; ++j) {
			const MeshRange& m = ranges[j];

			blasEntry.geometries[j].setGeometryType(vk::GeometryTypeKHR::eTriangles)
												.setFlags(vk::GeometryFlagBitsKHR::eOpaque)
												.geometry.setTriangles(triData);

			blasEntry.geometries[j].geometry.triangles.maxVertex = m.count;

			blasEntry.ranges[j].firstVertex		= m.base;
			blasEntry.ranges[j].primitiveOffset = m.start *(indexData.type == vk::IndexType::eUint32 ? sizeof(uint32_t) : sizeof(uint16_t));

			size_t elementCount = m.count / 3;
			//TODO: isone of these wrong...
			blasEntry.ranges[j].primitiveCount	= elementCount;
			blasEntry.maxPrims[j] = elementCount;
		}
	}

	vk::DeviceSize totalSize	= 0;
	vk::DeviceSize scratchSize	= 0;

	for (auto& i : m_blasBuildInfo) {	//Go through each of the added entries to build up data...
		i.buildInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
		i.buildInfo.mode = vk::BuildAccelerationStructureModeKHR::eBuild;
		i.buildInfo.geometryCount	= i.geometries.size(); //TODO
		i.buildInfo.pGeometries		= i.geometries.data();
		i.buildInfo.flags |= inFlags;

		m_device.getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice,
			&i.buildInfo, i.maxPrims.data(), &i.sizeInfo);

		totalSize	+= i.sizeInfo.accelerationStructureSize;
		scratchSize  = std::max(scratchSize, i.sizeInfo.buildScratchSize);
	}

	VKQuick::Buffer scratchBuffer = m_memoryManager.CreateBuffer(
		{
			.size	= scratchSize,
			.usage	= vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eStorageBuffer,
		},
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		"Scratch Buffer"
	);

	vk::UniqueCommandBuffer buffer = CmdBufferCreateBegin(m_device, m_pool, "Making BLAS");

	for (auto& i : m_blasBuildInfo) {		//Make the buffer for each blas entry...
		vk::AccelerationStructureCreateInfoKHR createInfo;
		createInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
		createInfo.size = i.sizeInfo.accelerationStructureSize;

		i.buffer = m_memoryManager.CreateBuffer(
			{
				.size	= createInfo.size,
				.usage	=	vk::BufferUsageFlagBits::eShaderDeviceAddress | 
							vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR,
			},
			vk::MemoryPropertyFlagBits::eHostVisible,
			"BLAS Buffer"
		);

		createInfo.buffer = i.buffer;

		i.accelStructure = m_device.createAccelerationStructureKHRUnique(createInfo);

		i.buildInfo.dstAccelerationStructure	= *i.accelStructure;
		i.buildInfo.scratchData.deviceAddress	= scratchBuffer.GetDeviceAddress();

		const vk::AccelerationStructureBuildRangeInfoKHR* rangeInfo = i.ranges.data();

		buffer->buildAccelerationStructuresKHR(1, &i.buildInfo, &rangeInfo);
					
		buffer->pipelineBarrier(
			vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR, //Source
			vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR, //Dest
			{}, //Dependency Flags
			{//MemoryBarriers
				{
					.srcAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteKHR,
					.dstAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteKHR
				}
				},
			{}, //bufferMemoryBarriers
			{} //imageMemoryBarriers
		);
	}
	VKQuick::CmdBufferSubmit(
		{
			.buffer = *buffer,
			.queue = m_queue,
			.device = m_device,
			.wait = true
		}
	);
}

void BVHBuilder::BuildTLAS(vk::BuildAccelerationStructureFlagsKHR flags) {
	std::vector<vk::AccelerationStructureInstanceKHR> tlasEntries;

	const uint32_t instanceCount = m_entries.size();

	tlasEntries.resize(instanceCount);

	for (int i = 0; i < instanceCount; ++i) {
		tlasEntries[i].transform = m_entries[i].transform;

		uint32_t meshID = m_entries[i].meshID;

		tlasEntries[i].accelerationStructureReference = m_device.getBufferAddress({ .buffer = m_blasBuildInfo[meshID].buffer });

		tlasEntries[i].flags = (VkGeometryInstanceFlagBitsKHR)vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable;
		tlasEntries[i].mask = m_entries[i].mask;
		tlasEntries[i].instanceShaderBindingTableRecordOffset = m_entries[i].hitID;
		tlasEntries[i].instanceCustomIndex = (meshID & ((1 << 24)-1)) | (m_entries[i].customIndex << 24);
	}

	size_t dataSize = instanceCount * sizeof(vk::AccelerationStructureInstanceKHR);

	VKQuick::Buffer instanceBuffer = m_memoryManager.CreateBuffer(
		{
			.size  = dataSize,
			.usage =	vk::BufferUsageFlagBits::eShaderDeviceAddress | 
						vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
						vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR
		},
		vk::MemoryPropertyFlagBits::eHostVisible,
		"Instance Buffer"
	);

	instanceBuffer.CopyData(tlasEntries.data(), dataSize);

	vk::AccelerationStructureGeometryKHR tlasGeometry;
	tlasGeometry.geometryType = vk::GeometryTypeKHR::eInstances;
	tlasGeometry.geometry = vk::AccelerationStructureGeometryInstancesDataKHR();
	tlasGeometry.geometry.instances.data = instanceBuffer.GetDeviceAddress();

	vk::AccelerationStructureBuildGeometryInfoKHR geomInfo;
	geomInfo.flags			= flags;
	geomInfo.geometryCount	= 1;
	geomInfo.pGeometries	= &tlasGeometry;
	geomInfo.mode = vk::BuildAccelerationStructureModeKHR::eBuild;
	geomInfo.type = vk::AccelerationStructureTypeKHR::eTopLevel;
	geomInfo.srcAccelerationStructure = nullptr; //??

	vk::AccelerationStructureBuildSizesInfoKHR sizesInfo;
	m_device.getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, &geomInfo, &instanceCount, &sizesInfo);

	m_tlasBuffer = m_memoryManager.CreateBuffer(
		{
			.size	= sizesInfo.accelerationStructureSize,
			.usage	=	vk::BufferUsageFlagBits::eShaderDeviceAddress | 
						vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR,
		},
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		"Instance Buffer"
	);


	vk::AccelerationStructureCreateInfoKHR tlasCreateInfo;
	tlasCreateInfo.buffer = m_tlasBuffer;
	tlasCreateInfo.size = sizesInfo.accelerationStructureSize;
	tlasCreateInfo.type = vk::AccelerationStructureTypeKHR::eTopLevel;
	
	m_tlas = m_device.createAccelerationStructureKHRUnique(tlasCreateInfo);

	VKQuick::Buffer scratchBuffer = m_memoryManager.CreateBuffer(
		{
			.size	= sizesInfo.buildScratchSize,
			.usage	=	vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
						vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR |
						vk::BufferUsageFlagBits::eStorageBuffer |
						vk::BufferUsageFlagBits::eShaderDeviceAddress
		},
		vk::MemoryPropertyFlagBits::eHostVisible,
		"Instance Buffer"
	);

	vk::DeviceAddress scratchAddr = scratchBuffer.GetDeviceAddress();

	geomInfo.srcAccelerationStructure = nullptr;
	geomInfo.dstAccelerationStructure = *m_tlas;
	geomInfo.scratchData.deviceAddress = scratchAddr;

	vk::AccelerationStructureBuildRangeInfoKHR rangeInfo;
	rangeInfo.primitiveCount = instanceCount;

	vk::AccelerationStructureBuildRangeInfoKHR* rangeInfoPtr = &rangeInfo;

	vk::UniqueCommandBuffer cmdBuffer = CmdBufferCreateBegin(m_device, m_pool, "Making TLAS");
	cmdBuffer->buildAccelerationStructuresKHR(1, &geomInfo, &rangeInfoPtr);
	VKQuick::CmdBufferSubmit(
		{
			.buffer = *cmdBuffer,
			.queue	= m_queue,
			.device = m_device,
			.wait	= true
		}
	);
}