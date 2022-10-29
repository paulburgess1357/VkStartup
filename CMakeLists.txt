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

target_compile_options(${TargetName} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wno-unknown-pragmas -Wextra -Wpedantic -Werror>
)

set(IncludeDirectories 
	${CMAKE_CURRENT_SOURCE_DIR}/${TargetName} 
)
set(LinkDirectories "")

set(ConanSetupHasRun False)
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	message("Using Conan for dependency management: ${TargetName}")
	# Conan setup has not run AND this repo is not a shared fetch repo
	if(NOT ${ConanSetupHasRun} AND NOT DEFINED VkStartupFetchContentRepo)
		message("Loading Conan macros")
		include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
		conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
		set(ConanSetupHasRun True)
	endif()
	set(CONAN_INCLUDE_DIRS_FILTERED ${CONAN_INCLUDE_DIRS})

	# Filter out packages not necessary for main library
	list(REMOVE_ITEM CONAN_INCLUDE_DIRS_FILTERED 
		${CONAN_INCLUDE_DIRS_GLFW}
	)

	list(APPEND IncludeDirectories ${CONAN_INCLUDE_DIRS_FILTERED})
	list(APPEND LinkDirectories 
		CONAN_PKG::vulkan-loader 
		CONAN_PKG::vulkan-memory-allocator)
else()
	find_package(Vulkan REQUIRED)
	list(APPEND IncludeDirectories ${Vulkan_INCLUDE_DIR})
	list(APPEND LinkDirectories ${Vulkan_LIBRARIES})

	find_package(VulkanMemoryAllocator REQUIRED)
	target_link_libraries(${TargetName} PRIVATE VulkanMemoryAllocator)
endif()

# Clone VkShared
set(DEP_VkShared VkShared)
if (NOT TARGET ${DEP_VkShared})
	message("\nCloning VkShared... (GitHub SSH key setup required)")
	FetchContent_Declare(
	${DEP_VkShared}
    #GIT_REPOSITORY git@github.com:paulburgess1357/VkShared.git
    SOURCE_DIR "C:/Users/paulb/Desktop/git projects vulkan/VkShared"
	GIT_TAG        master
	)
	set(VkSharedFetchContentRepo True)
	FetchContent_MakeAvailable(${DEP_VkShared})
endif()

# Include and link VkShared
list(APPEND IncludeDirectories 
	${VkShared_SOURCE_DIR}/${DEP_VkShared}
)
list(APPEND LinkDirectories 
	${DEP_VkShared}
)

target_include_directories(${TargetName} PRIVATE 
	${IncludeDirectories}
)
target_link_libraries(${TargetName} PRIVATE
	${LinkDirectories}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HeaderFiles})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})

# VkStartupTest
if(NOT DEFINED VkStartupFetchContentRepo)
	add_subdirectory(VkStartupTest)
endif()

install(TARGETS ${TargetName})
