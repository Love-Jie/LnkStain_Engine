cmake_minimum_required(VERSION 3.29)

#shader file
set(SHADERS
        00_hello_triangle.vert
        00_hello_triangle.frag
        01_hello_buffer.vert
        01_hello_buffer.frag
        02_descriptor_set.vert
        02_descriptor_set.frag
        03_unlit_material.vert
        03_unlit_material.frag
)

spirv_shaders(
        SHADER_HEADERS
        SPIRV_VERSION
        1.0   #current targeting Vulkan 1.0
        SOURCES
        ${SHADERS}
)

add_executable(SandBox
        ${SHADER_HEADERS}
        Main.cpp
        LSEventTesting.cpp
)

target_link_libraries(SandBox PRIVATE lnkstain_core)
