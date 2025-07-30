file(GLOB_RECURSE CPPS  ./src/*.cpp )
file(GLOB_RECURSE HPPS  ./include/*.hpp )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Find OpenCL
find_package(OpenCL)
if(OpenCL_FOUND)
    message(STATUS "OpenCL found: ${OpenCL_INCLUDE_DIRS}")
    add_definitions(-DHAS_OPENCL)
else()
    message(WARNING "OpenCL not found. OpenCL compilation will be disabled.")
endif()

# Find CUDA
include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    find_package(CUDAToolkit)
    if(CUDAToolkit_FOUND)
        message(STATUS "CUDA found: ${CUDAToolkit_INCLUDE_DIRS}")
        add_definitions(-DHAS_CUDA)
    endif()
else()
    message(WARNING "CUDA not found. CUDA compilation will be disabled.")
endif()

add_executable(epen_graph_editor ${CPPS} ${HPPS})

target_include_directories(epen_graph_editor
  PUBLIC
    include
)

# Base libraries
set(BASE_LIBS QtPropertyBrowser QtNodes Qt6::Core Qt6::Widgets ${QSCINTILLA_TARGET_NAME})

# Add OpenCL if found
if(OpenCL_FOUND)
    target_include_directories(epen_graph_editor PRIVATE ${OpenCL_INCLUDE_DIRS})
    list(APPEND BASE_LIBS ${OpenCL_LIBRARIES})
endif()

# Add CUDA if found
if(CUDAToolkit_FOUND)
    target_include_directories(epen_graph_editor PRIVATE ${CUDAToolkit_INCLUDE_DIRS})
    list(APPEND BASE_LIBS CUDA::cudart CUDA::cuda_driver CUDA::nvrtc)
endif()

target_link_libraries(epen_graph_editor PRIVATE ${BASE_LIBS})

# Platform-specific settings
if(APPLE)
    # macOS has OpenCL as part of the system frameworks
    if(NOT OpenCL_FOUND)
        find_library(OpenCL_LIBRARY OpenCL)
        if(OpenCL_LIBRARY)
            target_link_libraries(epen_graph_editor PRIVATE ${OpenCL_LIBRARY})
            add_definitions(-DHAS_OPENCL)
        endif()
    endif()
    # Metal support
    find_library(METAL_LIBRARY Metal)
    find_library(METALKIT_LIBRARY MetalKit)
    if(METAL_LIBRARY AND METALKIT_LIBRARY)
        target_link_libraries(epen_graph_editor PRIVATE ${METAL_LIBRARY} ${METALKIT_LIBRARY})
        add_definitions(-DHAS_METAL)
    endif()
endif()

# Always enable GLSL as fallback
add_definitions(-DHAS_GLSL)