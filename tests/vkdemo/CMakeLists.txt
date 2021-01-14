project(SQEE_VKDEMO CXX)

################################################################################

function (add_shader TARGET SHADER)
    add_custom_command (
           OUTPUT ${SHADER}.spv
           COMMAND glslc -o ${SHADER}.spv ${SHADER}
           DEPENDS ${SHADER}
           IMPLICIT_DEPENDS CXX ${SHADER}
           VERBATIM
    )
    set_source_files_properties(${SHADER}.spv PROPERTIES GENERATED True)
    target_sources(${TARGET} PRIVATE ${SHADER}.spv)
endfunction (add_shader)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(sqee-vkdemo ${HEADERS} ${SOURCES})

set_property(TARGET sqee-vkdemo PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee-vkdemo PROPERTY CXX_STANDARD_REQUIRED True)

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(sqee-vkdemo PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(sqee-vkdemo PRIVATE /W3 /wd4251)
endif ()

add_shader(sqee-vkdemo "${PROJECT_SOURCE_DIR}/shaders/screen.vert")
add_shader(sqee-vkdemo "${PROJECT_SOURCE_DIR}/shaders/screen.frag")

target_link_libraries(sqee-vkdemo PRIVATE sqee)

################################################################################

#add_custom_command(TARGET sqee-vkdemo POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
#    "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/tests/vkdemo/assets")

#add_custom_command(TARGET sqee-vkdemo POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
#    "${PROJECT_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/tests/vkdemo/shaders")
