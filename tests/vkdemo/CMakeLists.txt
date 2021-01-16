project(SQEE_VKDEMO CXX)

################################################################################

set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/tests/vkdemo/shaders")

function (target_add_shaders TARGET SHADERS)
    foreach (SHADER ${SHADERS})

        string( REGEX REPLACE
                "${PROJECT_SOURCE_DIR}/shaders/(.*)\.(vert|geom|frag)"
                "${SHADER_OUTPUT_DIR}/\\1\.\\2\.spv"
                OUTPATH ${SHADER} )

        if (${SHADER} STREQUAL ${OUTPATH})
            message(FATAL_ERROR "bad shader path '${SHADER}'")
        endif ()

        get_filename_component(OUTDIR ${OUTPATH} DIRECTORY)
        file(MAKE_DIRECTORY ${OUTDIR})

        add_custom_command( OUTPUT ${OUTPATH}
                            COMMAND glslc -o ${OUTPATH} ${SHADER}
                            DEPENDS ${SHADER}
                            IMPLICIT_DEPENDS CXX ${SHADER}
                            VERBATIM )

        target_sources(${TARGET} PRIVATE ${SHADER})
        source_group("GLSL" FILES ${SHADER})

        set_source_files_properties(${OUTPATH} PROPERTIES GENERATED True)
        target_sources(${TARGET} PRIVATE ${OUTPATH})

    endforeach ()
endfunction ()

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(sqee-vkdemo ${HEADERS} ${SOURCES})

file(GLOB_RECURSE VERT_SHADERS "${PROJECT_SOURCE_DIR}/shaders/*.vert")
file(GLOB_RECURSE FRAG_SHADERS "${PROJECT_SOURCE_DIR}/shaders/*.frag")

target_add_shaders(sqee-vkdemo ${VERT_SHADERS})
target_add_shaders(sqee-vkdemo ${FRAG_SHADERS})

################################################################################

set_property(TARGET sqee-vkdemo PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee-vkdemo PROPERTY CXX_STANDARD_REQUIRED True)

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(sqee-vkdemo PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(sqee-vkdemo PRIVATE /W3 /wd4251)
endif ()

target_link_libraries(sqee-vkdemo PRIVATE sqee)

################################################################################

#add_custom_command(TARGET sqee-vkdemo POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
#    "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/tests/vkdemo/assets")

#add_custom_command(TARGET sqee-vkdemo POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
#    "${PROJECT_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/tests/vkdemo/shaders")
