cmake_minimum_required(VERSION 3.16)
project(Wren C)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/wren.h"
                          "${PROJECT_SOURCE_DIR}/include/wren.hpp"
                          "${PROJECT_SOURCE_DIR}/source/*.h")

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.c")

if (SQEE_STATIC_LIB)
    add_library(wren STATIC ${HEADERS} ${SOURCES})
    target_compile_definitions(wren PUBLIC SQEE_WREN_STATIC)
else ()
    add_library(wren SHARED ${HEADERS} ${SOURCES})
    target_compile_definitions(wren PRIVATE SQEE_WREN_EXPORT)
    if (SQEE_GNU OR SQEE_CLANG)
        target_compile_options(wren PRIVATE -fvisibility=hidden)
    endif ()
endif ()

target_include_directories(wren PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_property(TARGET wren PROPERTY C_STANDARD 99)
set_property(TARGET wren PROPERTY C_STANDARD_REQUIRED True)

# force use of the static msvc runtime so we don't need a bunch of extra dlls
set_property(TARGET wren PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

################################################################################

target_compile_definitions(wren PRIVATE WREN_OPT_META=0)
target_compile_definitions(wren PRIVATE WREN_OPT_RANDOM=0)

# uncomment for easier debugging of wren itself
#target_compile_definitions(wren PRIVATE WREN_NAN_TAGGING=0)

# for some reason wren uses DEBUG rather than NDEBUG
target_compile_definitions(wren PRIVATE $<$<CONFIG:Debug>:DEBUG>)

################################################################################

if (SQEE_LINUX)
    target_compile_definitions(wren PRIVATE SQEE_LINUX)
elseif (SQEE_WINDOWS)
    target_compile_definitions(wren PRIVATE SQEE_WINDOWS)
else ()
    message(FATAL_ERROR "could not detect platform")
endif ()

if (SQEE_GNU)
    target_compile_definitions(wren PRIVATE SQEE_GNU)
elseif (SQEE_CLANG)
    target_compile_definitions(wren PRIVATE SQEE_CLANG)
elseif (SQEE_MSVC)
    target_compile_definitions(wren PRIVATE SQEE_MSVC)
else ()
    message(FATAL_ERROR "could not detect compiler")
endif ()

################################################################################

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(wren PRIVATE -Wall -Wno-unused-variable)
elseif (SQEE_MSVC)
    target_compile_options(wren PRIVATE /W3 /wd4996)
endif ()
