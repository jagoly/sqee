################################################################################

# set some basic option that we always want when using MSVC
function (target_set_msvc_options TARGET)

    if (SQEE_STATIC_LIB)
        set_property(TARGET ${TARGET} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
    else ()
        set_property(TARGET ${TARGET} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)
    endif ()

    target_link_options(${TARGET} PRIVATE /INCREMENTAL:NO)

endfunction ()

################################################################################

# manually set all output directories becasue cmake is a pain
function (target_set_output_directory TARGET DIR)

    if (CMAKE_CONFIGURATION_TYPES)
        set_property(TARGET ${TARGET} PROPERTY LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${DIR}")
        set_property(TARGET ${TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${DIR}")
        set_property(TARGET ${TARGET} PROPERTY PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${DIR}")
        set_property(TARGET ${TARGET} PROPERTY OUTPUT_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>/${DIR}")
    else ()
        set_property(TARGET ${TARGET} PROPERTY LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${DIR}")
        set_property(TARGET ${TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${DIR}")
        set_property(TARGET ${TARGET} PROPERTY PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${DIR}")
        set_property(TARGET ${TARGET} PROPERTY OUTPUT_DIR "${CMAKE_BINARY_DIR}/out/${DIR}")
    endif ()

endfunction ()

################################################################################

# add glsl source files that will be compiled to spir-v
function (target_add_shaders TARGET)

    get_target_property(OUTPUT_DIR ${TARGET} OUTPUT_DIR)

    foreach (SHADER ${ARGN})
        string( REGEX REPLACE
                "${PROJECT_SOURCE_DIR}/shaders/(.*)\.(vert|geom|frag)"
                "${OUTPUT_DIR}/shaders/\\1\.\\2\.spv"
                OUTPATH ${SHADER} )

        # if shader has a bad extension then the regex will have failed
        if (${SHADER} STREQUAL ${OUTPATH})
            message(FATAL_ERROR "bad shader path '${SHADER}'")
        endif ()

        get_filename_component(OUTDIR ${OUTPATH} DIRECTORY)

        add_custom_command( OUTPUT ${OUTPATH}
                            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTDIR}
                            COMMAND glslc --target-env=vulkan1.2 ${SHADER} -o ${OUTPATH}
                            DEPENDS ${SHADER}
                            IMPLICIT_DEPENDS CXX ${SHADER}
                            VERBATIM )

        set_source_files_properties(${OUTPATH} PROPERTIES GENERATED True)
        target_sources(${TARGET} PRIVATE ${OUTPATH})
    endforeach ()

endfunction ()

################################################################################

# create a link to or copy of a directory in the output directory
function (target_link_or_copy_directory TARGET DIR)

    get_target_property(OUTPUT_DIR ${TARGET} OUTPUT_DIR)

    # usually don't have permission to create symlinks on windows
    if (SQEE_WINDOWS)
        add_custom_command( TARGET ${TARGET} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
                            rm -rf "${OUTPUT_DIR}/${DIR}" )
        add_custom_command( TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                            copy_directory "${PROJECT_SOURCE_DIR}/${DIR}" "${OUTPUT_DIR}/${DIR}" )
    else ()
        add_custom_command( TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                            create_symlink "${PROJECT_SOURCE_DIR}/${DIR}" "${OUTPUT_DIR}/${DIR}" )
    endif ()

endfunction ()
