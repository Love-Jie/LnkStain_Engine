set(GLSLANGVALIDATOR_COMMAND "D:/VulkanSDK/Bin/glslangValidator.exe")
find_program(GLSLANGVALIDATOR_COMMAND glslandValidator)
if(NOT GLSLANGVALIDATOR_COMMAND)
    message(FATAL_ERROR "glslValidator required - source maintained at....")
endif()

function(spirv_shaders ret)
    set(options)
    set(oneValueArgs SPIRV_VERSION)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(_spirvshaders "${options}" "${oneValueArgs}"
             "${multiValueArgs}" ${ARGN})

    if(NOT _spirvshaders_SPIRV_VERSION)
        set(_spirvshaders_SPIRV_VERSION 1.0)
    endif()

    foreach(GLSL ${_spirvshaders_SOURCES})
        string(MAKE_C_IDENTIFIER ${GLSL} IDENTIFIER)
        set(HEADER "${LS_DEFINE_RES_ROOT_DIR}Shader/${GLSL}.spv")
        set(GLSL "${LS_DEFINE_RES_ROOT_DIR}Shader/${GLSL}")

        message("GLSL Command: ${GLSLANGVALIDATOR_COMMAND} -V --target-env spirv${_spirvshaders_SPIRV_VERSION} ${GLSL} -o ${HEADER}")


        add_custom_command(
            OUTPUT ${HEADER}
            COMMAND ${GLSLANGVALIDATOR_COMMAND} -V --target-env spirv${_spirvshaders_SPIRV_VERSION} ${GLSL} -o ${HEADER}
            DEPENDS ${GLSL})
        list(APPEND HEADERS ${HEADER})
    endforeach()

    set(${ret} "${HEADERS}" PARENT_SCOPE)
endfunction()