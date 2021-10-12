project(SQEE_VKDEMO CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(sqee-vkdemo ${HEADERS} ${SOURCES})

target_set_output_directory(sqee-vkdemo "tests/vkdemo")

file(GLOB_RECURSE VERT_SHADERS "${PROJECT_SOURCE_DIR}/shaders/*.vert")
file(GLOB_RECURSE FRAG_SHADERS "${PROJECT_SOURCE_DIR}/shaders/*.frag")
file(GLOB_RECURSE GLSL_HEADERS "${PROJECT_SOURCE_DIR}/shaders/*.glsl")

target_add_shaders(sqee-vkdemo ${VERT_SHADERS} ${FRAG_SHADERS})

target_sources(sqee-vkdemo PRIVATE ${VERT_SHADERS} ${FRAG_SHADERS} ${GLSL_HEADERS})
source_group("GLSL" FILES ${VERT_SHADERS} ${FRAG_SHADERS} ${GLSL_HEADERS})

target_link_or_copy_directory(sqee-vkdemo "assets")

################################################################################

set_property(TARGET sqee-vkdemo PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee-vkdemo PROPERTY CXX_STANDARD_REQUIRED True)

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(sqee-vkdemo PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(sqee-vkdemo PRIVATE /W3 /wd4251)
    target_set_msvc_options(sqee-vkdemo)
endif ()

target_link_libraries(sqee-vkdemo PRIVATE sqee)
