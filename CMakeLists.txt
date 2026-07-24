set(PROJECT_NAME VKQuick)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "Utils.h"
    "Buffer.h"
    "ComputePipelineBuilder.h"
    "DescriptorSetLayoutBuilder.h"
    "DynamicRenderBuilder.h"
	"TextureBuilder.h"
    #"Mesh.h"
    "Pipeline.h"
	"PipelineBuilderBase.h"
    "PipelineBuilder.h"
    "Instance.h"
    "RenderPassBuilder.h"
    "ShaderModule.h"
    "Texture.h"
    "DescriptorSetWriter.h"
    "DescriptorSetBinder.h"
	"DescriptorBufferWriter.h"
	"BVHBuilder.h"
	"RayTracingPipelineBuilder.h"	
	"ShaderBindingTableBuilder.h"
    "MemoryManager.h"
    "VMAMemoryManager.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
	"Utils.cpp"
    "Buffer.cpp"
    "ComputePipelineBuilder.cpp"
    "DescriptorSetLayoutBuilder.cpp"
    "DynamicRenderBuilder.cpp"
	"TextureBuilder.cpp"
    #"Mesh.cpp"
    "PipelineBuilder.cpp"
    "Instance.cpp"
    "RenderPassBuilder.cpp"
    "ShaderModule.cpp"
    "Texture.cpp"
	"BVHBuilder.cpp"
	"RayTracingPipelineBuilder.cpp"
	"ShaderBindingTableBuilder.cpp"	
    "VMAMemoryManager.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(SPIRV_Reflect_Files
	"${Vulkan_INCLUDE_DIR}../../Source/SPIRV-Reflect/SPIRV_Reflect.h" 
    "${Vulkan_INCLUDE_DIR}../../Source/SPIRV-Reflect/SPIRV_Reflect.c"
)
source_group("SPIRV Reflect" FILES ${SPIRV_Reflect_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${SPIRV_Reflect_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:<vulkan/vulkan.hpp$<ANGLE-R>>"
	"$<$<COMPILE_LANGUAGE:CXX>:<vma/vk_mem_alloc.h$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<memory$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<unordered_set$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<map$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<vector$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<set$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<string$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<fstream$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<iostream$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<string$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<iosfwd$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<functional$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<filesystem$<ANGLE-R>>"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

add_compile_definitions(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
add_compile_definitions(VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL=1)
add_compile_definitions(VK_NO_PROTOTYPES)
add_compile_definitions(VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

################################################################################
# Compile and link options
################################################################################
target_include_directories (${PROJECT_NAME} 
    PUBLIC ${Vulkan_INCLUDE_DIR}
    PUBLIC ${Vulkan_INCLUDE_DIR}../../Source/
)	

################################################################################
# Dependencies
################################################################################
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})