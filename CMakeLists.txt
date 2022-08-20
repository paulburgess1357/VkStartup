set(TargetName "VkStartup")
message("\nBuilding " ${TargetName})

cmake_minimum_required(VERSION 3.9.0)
project(${TargetName})
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${TargetName}/*.cpp")
file(GLOB_RECURSE HeaderFiles CONFIGURE_DEPENDS "${TargetName}/*.h")

add_library(${TargetName} STATIC ${SourceFiles} ${HeaderFiles})
set_target_properties(${TargetName} PROPERTIES DEBUG_POSTFIX d)

set(DEP_VkShared VkShared)
message("\nCloning VkShared...")
FetchContent_Declare(
 ${DEP_VkShared}
  GIT_REPOSITORY https://github.com/paulburgess1357/VkShared.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(${DEP_VkShared})

set(USE_CONAN FALSE)
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	set(USE_CONAN TRUE)
endif()

if(USE_CONAN)
	message("Using Conan for dependency management")
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup(TARGETS)
	target_include_directories(${TargetName} PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/${TargetName}
		${CONAN_INCLUDE_DIRS}
		${VkShared_SOURCE_DIR}/${DEP_VkShared}
	)
    target_link_libraries(${TargetName} 
        CONAN_PKG::vulkan-loader
		${DEP_VkShared}
    )
else()
	find_package(Vulkan REQUIRED)
	
	target_include_directories(${TargetName} PRIVATE 
	    ${CMAKE_CURRENT_SOURCE_DIR}/${TargetName}
		${Vulkan_INCLUDE_DIR}
		${VkShared_SOURCE_DIR}/${DEP_VkShared}
	)
	target_link_libraries(${TargetName} 
		${Vulkan_LIBRARIES}
		${DEP_VkShared}
	)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HeaderFiles})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})

# Testing
add_subdirectory(VkStartupTest)

install(TARGETS ${TargetName})
