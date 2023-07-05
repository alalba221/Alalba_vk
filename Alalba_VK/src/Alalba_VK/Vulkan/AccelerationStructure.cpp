#include "pch.h"
#include "AccelerationStructure.h"
#include "Device.h"
#include "Buffer.h"
#include "Alalba_VK/Assets/Vertex.h"
#include "CommandPool.h"
#include "CommandBuffers.h"

namespace vk
{
	// function pointer
	static PFN_vkGetBufferDeviceAddressKHR fpGetBufferDeviceAddressKHR;//
	static PFN_vkCreateAccelerationStructureKHR fpCreateAccelerationStructureKHR;//
	static PFN_vkDestroyAccelerationStructureKHR fpDestroyAccelerationStructureKHR;//
	static PFN_vkGetAccelerationStructureBuildSizesKHR fpGetAccelerationStructureBuildSizesKHR;//
	static PFN_vkGetAccelerationStructureDeviceAddressKHR fpGetAccelerationStructureDeviceAddressKHR;//
	static PFN_vkCmdBuildAccelerationStructuresKHR fpCmdBuildAccelerationStructuresKHR;//
	static PFN_vkBuildAccelerationStructuresKHR fpBuildAccelerationStructuresKHR;
	static PFN_vkCmdTraceRaysKHR fpCmdTraceRaysKHR;
	static PFN_vkGetRayTracingShaderGroupHandlesKHR fpGetRayTracingShaderGroupHandlesKHR;
	static PFN_vkCreateRayTracingPipelinesKHR fpCreateRayTracingPipelinesKHR;

	BLAS::BLAS(const Device& device, const Buffer& vertexBuffer, const Buffer& indexBuffer, 
		const uint32_t vertexCount,const uint32_t indexCount, Allocator& allocator, const std::string& tag)
		:m_device(device), m_allocator(allocator), m_tag(tag)
	{
		ALALBA_INFO("Create Blas: {0}",m_tag);

		GET_DEVICE_PROC_ADDR(m_device.Handle(), GetBufferDeviceAddressKHR);
		GET_DEVICE_PROC_ADDR(m_device.Handle(), CreateAccelerationStructureKHR);
		GET_DEVICE_PROC_ADDR(m_device.Handle(), GetAccelerationStructureBuildSizesKHR);
		GET_DEVICE_PROC_ADDR(m_device.Handle(), GetAccelerationStructureDeviceAddressKHR);
		GET_DEVICE_PROC_ADDR(m_device.Handle(), CmdBuildAccelerationStructuresKHR);
		GET_DEVICE_PROC_ADDR(m_device.Handle(), DestroyAccelerationStructureKHR);

		// BLAS builder requires raw device addresses.
		VkDeviceAddress vertexBufferAddress = vertexBuffer.DeviceAddress();
		VkDeviceAddress indexBufferAddress = indexBuffer.DeviceAddress();
		
		// Describe buffer as array of Vertex.
		VkAccelerationStructureGeometryTrianglesDataKHR triangles{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR };
		triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;  // vec3 vertex position data.
		triangles.vertexData.deviceAddress = vertexBufferAddress;
		triangles.vertexStride = sizeof(Vertex);
		// Describe index data (32-bit unsigned int)
		triangles.indexType = VK_INDEX_TYPE_UINT32;
		triangles.indexData.deviceAddress = indexBufferAddress;
		// Indicate identity transform by setting transformData to null device pointer.
		triangles.transformData = {};
		triangles.maxVertex = vertexCount;

		// data pointer
	
		m_accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		m_accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		m_accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		m_accelerationStructureGeometry.geometry.triangles = triangles;
		//m_accelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		//m_accelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		//m_accelerationStructureGeometry.geometry.triangles.vertexData.deviceAddress = vertexBufferAddress;
		//m_accelerationStructureGeometry.geometry.triangles.maxVertex = vertexCount;
		//m_accelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
		//m_accelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		//m_accelerationStructureGeometry.geometry.triangles.indexData.deviceAddress = indexBufferAddress;
		//m_accelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = 0;
		//m_accelerationStructureGeometry.geometry.triangles.transformData.hostAddress = nullptr;
		////accelerationStructureGeometry.geometry.triangles.transformData = transformBufferDeviceAddress;


	/*
	 * Creating a Bottom-Level-Accelerated-Structure, requires the following elements:

	 * VkAccelerationStructureBuildGeometryInfoKHR` : to create and build the acceleration structure. It is referencing the array of `VkAccelerationStructureGeometryKHR` created in `objectToVkGeometryKHR()`
	 * VkAccelerationStructureBuildRangeInfoKHR`: a reference to the range, also created in `objectToVkGeometryKHR()`
	 * VkAccelerationStructureBuildSizesInfoKHR`: the size require for the creation of the AS and the scratch buffer
	*/

		VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
		accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		/// Our blas is made from only one geometry, but could be made of many geometries	
		accelerationStructureBuildGeometryInfo.geometryCount = 1;
		accelerationStructureBuildGeometryInfo.pGeometries = &m_accelerationStructureGeometry;


		// get size
		m_primitiveCount = static_cast<uint32_t>(indexCount)/3;
		VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
		fpGetAccelerationStructureBuildSizesKHR(
			m_device.Handle(),
			VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
			&accelerationStructureBuildGeometryInfo,
			&m_primitiveCount,
			&accelerationStructureBuildSizesInfo);
		
		// no compact
		m_scratchBufferSize = accelerationStructureBuildSizesInfo.buildScratchSize;
		
		/// Create
		// 1. 
		m_buffer = vk::Buffer::Builder(m_device, m_allocator)
			.SetTag("BLAS Buffer")
			.SetSize(accelerationStructureBuildSizesInfo.accelerationStructureSize)
			.SetUsage(VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();

		VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
		accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		accelerationStructureCreateInfo.buffer = m_buffer->Handle();
		accelerationStructureCreateInfo.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
		accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		VkResult err = fpCreateAccelerationStructureKHR(m_device.Handle(), &accelerationStructureCreateInfo, nullptr, &m_BLAS);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create BLAS Failed");

		VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
		accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		accelerationDeviceAddressInfo.accelerationStructure = m_BLAS;
		m_deviceAddress = fpGetAccelerationStructureDeviceAddressKHR(m_device.Handle(), &accelerationDeviceAddressInfo);

	}
	void BLAS::BuildOnDevice(const Queue& q, const CommandPool& cmdPool)
	{
		// Create a small scratch buffer used during build of the bottom level acceleration structure
		std::unique_ptr<Buffer>scratchBuffer = Buffer::Builder(m_device, m_allocator)
			.SetTag("BLAS scratch Buffer")
			.SetSize(m_scratchBufferSize)
			.SetUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();

		VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
		accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		accelerationBuildGeometryInfo.dstAccelerationStructure = m_BLAS;
		accelerationBuildGeometryInfo.geometryCount = 1;
		accelerationBuildGeometryInfo.pGeometries = &m_accelerationStructureGeometry;
		accelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer->DeviceAddress();

		VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
		accelerationStructureBuildRangeInfo.primitiveCount = m_primitiveCount;
		accelerationStructureBuildRangeInfo.primitiveOffset = 0;
		accelerationStructureBuildRangeInfo.firstVertex = 0;
		accelerationStructureBuildRangeInfo.transformOffset = 0;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &accelerationStructureBuildRangeInfo };

		// Build the acceleration structure on the device via a one-time command buffer submission
		// Some implementations may support acceleration structure building on the host (VkPhysicalDeviceAccelerationStructureFeaturesKHR->accelerationStructureHostCommands), but we prefer device builds
		std::unique_ptr<CommandBuffers>buildBLASCmdBuffer = CommandBuffers::Allocator(m_device, cmdPool)
			.SetTag("Command Buffers for BLAS Build: " + m_tag)
			.OneTimeSubmit(true)
			.SetSize(1)
			.Allocate();
		
		buildBLASCmdBuffer->BeginRecording(0);
		VkCommandBuffer cmdbuffer = (*buildBLASCmdBuffer)[0];
		fpCmdBuildAccelerationStructuresKHR(
			cmdbuffer,
			1,
			&accelerationBuildGeometryInfo,
			accelerationBuildStructureRangeInfos.data());
		buildBLASCmdBuffer->EndRecording(0);
		buildBLASCmdBuffer->Flush(0, q);

		scratchBuffer->Clean();
	}
	void BLAS::Clean()
	{
		if (m_BLAS != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean BLAS {0}", m_tag);
			fpDestroyAccelerationStructureKHR(m_device.Handle(), m_BLAS, nullptr);
			m_BLAS = VK_NULL_HANDLE;
		}
		m_buffer->Clean();
	}

	

	TLAS::Builder& TLAS::Builder::AddInstance(const BLAS& blas, const glm::mat4& transform, uint32_t id, uint32_t sbtOffset)
	{
		VkAccelerationStructureInstanceKHR instance{};
		//instance.transform = transformMatrix;
		instance.instanceCustomIndex = id;
		instance.mask = 0xFF;        //  Only be hit if rayMask & instance.mask != 0
		instance.instanceShaderBindingTableRecordOffset = 0;
		instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		instance.accelerationStructureReference = blas.m_deviceAddress;

		glm::mat4 transposed = glm::transpose(transform);
		std::memcpy(&instance.transform, &transposed, sizeof(instance.transform));

		m_instances.push_back(instance);
		return *this;
	}

	TLAS::TLAS(const Device& device, Allocator& allocator, const std::vector<VkAccelerationStructureInstanceKHR>& instances, const Queue& q, const CommandPool& cmdPool)
		:m_device(device),m_allocator(allocator)
	{
		std::unique_ptr<Buffer> instancesBuffer = Buffer::Builder(m_device, m_allocator)
			.SetTag("TLAS instances Buffer")
			.SetSize(instances.size()*sizeof(VkAccelerationStructureInstanceKHR))
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();

		instancesBuffer->CopyDataFrom(instances.data(), instances.size() * sizeof(VkAccelerationStructureInstanceKHR), q, cmdPool);


		VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
		accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		accelerationStructureGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		accelerationStructureGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
		accelerationStructureGeometry.geometry.instances.data.deviceAddress = instancesBuffer->DeviceAddress();

		// Get size info
		VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
		accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationStructureBuildGeometryInfo.geometryCount = 1;
		accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;

		uint32_t instanceCount = instances.size();
		VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR };
		fpGetAccelerationStructureBuildSizesKHR(
			m_device.Handle(),
			VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
			&accelerationStructureBuildGeometryInfo,
			&instanceCount,
			&accelerationStructureBuildSizesInfo);

		m_buffer = vk::Buffer::Builder(m_device, m_allocator)
			.SetTag("TLAS Buffer")
			.SetSize(accelerationStructureBuildSizesInfo.accelerationStructureSize)
			.SetUsage(VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();

		VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
		accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		accelerationStructureCreateInfo.buffer = m_buffer->Handle();
		accelerationStructureCreateInfo.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
		accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		fpCreateAccelerationStructureKHR(m_device.Handle(), &accelerationStructureCreateInfo, nullptr, &m_TLAS);


		// Create a small scratch buffer used during build of the top level acceleration structure
		std::unique_ptr<Buffer>scratchBuffer = Buffer::Builder(m_device, m_allocator)
			.SetTag("TLAS scratch Buffer")
			.SetSize(accelerationStructureBuildSizesInfo.buildScratchSize)
			.SetUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();

		VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
		accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		accelerationBuildGeometryInfo.dstAccelerationStructure = m_TLAS;
		accelerationBuildGeometryInfo.geometryCount = 1;
		accelerationBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;
		accelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer->DeviceAddress();

		VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
		accelerationStructureBuildRangeInfo.primitiveCount = instanceCount;
		accelerationStructureBuildRangeInfo.primitiveOffset = 0;
		accelerationStructureBuildRangeInfo.firstVertex = 0;
		accelerationStructureBuildRangeInfo.transformOffset = 0;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &accelerationStructureBuildRangeInfo };

		// Build the acceleration structure on the device via a one-time command buffer submission
		// Some implementations may support acceleration structure building on the host (VkPhysicalDeviceAccelerationStructureFeaturesKHR->accelerationStructureHostCommands), but we prefer device builds
		std::unique_ptr<CommandBuffers>buildTLASCmdBuffer = CommandBuffers::Allocator(m_device, cmdPool)
			.SetTag("Command Buffers for TLAS Build: " + m_tag)
			.OneTimeSubmit(true)
			.SetSize(1)
			.Allocate();

		buildTLASCmdBuffer->BeginRecording(0);
		VkCommandBuffer cmdbuffer = (*buildTLASCmdBuffer)[0];
		fpCmdBuildAccelerationStructuresKHR(
			cmdbuffer,
			1,
			&accelerationBuildGeometryInfo,
			accelerationBuildStructureRangeInfos.data());
		buildTLASCmdBuffer->EndRecording(0);
		buildTLASCmdBuffer->Flush(0, q);

		VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
		accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		accelerationDeviceAddressInfo.accelerationStructure = m_TLAS;
		m_deviceAddress = fpGetAccelerationStructureDeviceAddressKHR(m_device.Handle(), &accelerationDeviceAddressInfo);

		scratchBuffer->Clean();
		instancesBuffer->Clean();

	}

	void TLAS::Clean()
	{
		if (m_TLAS != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean TLAS {0}", m_tag);
			fpDestroyAccelerationStructureKHR(m_device.Handle(), m_TLAS, nullptr);
			m_TLAS = VK_NULL_HANDLE;
		}
		m_buffer->Clean();
	}

}

