cmake_minimum_required(VERSION 3.12)
project(Wren C)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/wren.h"
                          "${PROJECT_SOURCE_DIR}/include/wren.hpp"
                          "${PROJECT_SOURCE_DIR}/source/*.h")

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.c")

if (SQEE_STATIC_LIB)
    add_library(wren STATIC ${HEADERS} ${SOURCES})
else ()
    add_library(wren SHARED ${HEADERS} ${SOURCES})
endif ()

target_include_directories(wren PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_property(TARGET wren PROPERTY C_STANDARD 99)
set_property(TARGET wren PROPERTY C_STANDARD_REQUIRED True)

# force use of the static msvc runtime so we don't need a bunch of extra dlls
set_property(TARGET wren PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

# wren doesn't have any declspec stuff
set_property(TARGET wren PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS True)

################################################################################

target_compile_definitions(wren PRIVATE WREN_OPT_META=0)
target_compile_definitions(wren PRIVATE WREN_OPT_RANDOM=0)

target_compile_definitions(wren PUBLIC $<$<CONFIG:Debug>:DEBUG=1>)

################################################################################

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(wren PRIVATE -Wall)
elseif (SQEE_MSVC)
    target_compile_options(wren PRIVATE /W3 /wd4996)
else ()
    message(FATAL_ERROR "could not detect compiler")
endif ()