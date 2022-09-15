set(TargetName "VkStartupTest")
message("\nBuilding " ${TargetName})

cmake_minimum_required(VERSION 3.9.0)
project(${TargetName})
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${TargetName}/*.cpp")
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS "${TargetName}/*.h")

add_executable(${TargetName} ${SourceFiles} ${HeaderFiles})
set_target_properties(${TargetName} PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_compile_options(${TargetName} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wno-unknown-pragmas -Wextra -Wpedantic>
)

set(DEP_VkShared VkShared)
set(DEP_VkStartup VkStartup)

set(IncludeDirectories ${CMAKE_CURRENT_SOURCE_DIR}/${TargetName} ${VkShared_SOURCE_DIR}/${DEP_VkShared} ${CMAKE_SOURCE_DIR}/${DEP_VkStartup})
set(LinkDirectories ${DEP_VkShared} ${DEP_VkStartup})

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	message("Using Conan for dependency management")
	if(NOT ConanSetupHasRun)
		message("Loading Conan macros")
		include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
		conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
	endif()

	list(APPEND IncludeDirectories ${CONAN_INCLUDE_DIRS})
	list(APPEND LinkDirectories 
		CONAN_PKG::vulkan-loader 
		CONAN_PKG::glfw
		CONAN_PKG::vulkan-memory-allocator)
else()
	find_package(Vulkan REQUIRED)
	list(APPEND IncludeDirectories ${Vulkan_INCLUDE_DIR})
	list(APPEND LinkDirectories ${Vulkan_LIBRARIES})

	find_package(glfw3 3.3 REQUIRED)
	target_link_libraries(${TargetName} PRIVATE glfw)

	find_package(VulkanMemoryAllocator REQUIRED)
	target_link_libraries(${TargetName} PRIVATE VulkanMemoryAllocator)

endif()

target_include_directories(${TargetName} PRIVATE 
	${IncludeDirectories}
)
target_link_libraries(${TargetName} PRIVATE
	${LinkDirectories}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HeaderFiles})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})  

install(TARGETS ${TargetName})