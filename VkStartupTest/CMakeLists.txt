set(TargetName "VkStartupTest")
message("\nBuilding " ${TargetName})

cmake_minimum_required(VERSION 3.9.0)
project(${TargetName})
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${TargetName}/*.cpp")
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS "${TargetName}/*.h")

add_executable(${TargetName} ${SourceFiles} ${HeaderFiles})

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
	list(APPEND LinkDirectories CONAN_PKG::vulkan-loader)
else()
	find_package(Vulkan REQUIRED)
	list(APPEND IncludeDirectories ${Vulkan_INCLUDE_DIR})
	list(APPEND LinkDirectories ${Vulkan_LIBRARIES})
endif()

target_include_directories(${TargetName} PRIVATE 
	${IncludeDirectories}
)
target_link_libraries(${TargetName} 
	${LinkDirectories}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HeaderFiles})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})  

install(TARGETS ${TargetName})