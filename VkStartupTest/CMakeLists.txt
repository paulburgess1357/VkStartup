set(TargetName "VkStartupTest")
message("\nBuilding " ${TargetName})

cmake_minimum_required(VERSION 3.9.0)
project(${TargetName})
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${TargetName}/*.cpp")
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS "${TargetName}/*.h")

add_executable(${TargetName} ${SourceFiles} ${HeaderFiles})

set(USE_CONAN FALSE)
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	set(USE_CONAN TRUE)
endif()

set(DEP_VkShared VkShared)
set(DEP_VkStartup VkStartup)
if(USE_CONAN)
	message("Using Conan for dependency management")
	target_include_directories(${TargetName} PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/${TargetName}
		${CONAN_INCLUDE_DIRS}
		${VkShared_SOURCE_DIR}/${DEP_VkShared}
		${CMAKE_SOURCE_DIR}/${DEP_VkStartup}
	)
    target_link_libraries(${TargetName} 
        CONAN_PKG::vulkan-loader
		${DEP_VkShared}
		${DEP_VkStartup}
    )
else()
	find_package(Vulkan REQUIRED)
	
	target_include_directories(${TargetName} PRIVATE 
	    ${CMAKE_CURRENT_SOURCE_DIR}/${TargetName}
		${Vulkan_INCLUDE_DIR}
		${VkShared_SOURCE_DIR}/${DEP_VkShared}
		${CMAKE_SOURCE_DIR}/${DEP_VkStartup}
	)
	target_link_libraries(${TargetName} 
		${Vulkan_LIBRARIES}
		${DEP_VkShared}
		${DEP_VkStartup}
	)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HeaderFiles})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})  