cmake_minimum_required(VERSION 3.16)
project(FormatLib CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.cc")

if (SQEE_STATIC_LIB)
    add_library(format STATIC ${HEADERS} ${SOURCES})
else ()
    add_library(format SHARED ${HEADERS} ${SOURCES})
    target_compile_definitions(format PRIVATE FMT_EXPORT)
    target_compile_definitions(format PUBLIC FMT_SHARED)
endif ()

target_include_directories(format PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_property(TARGET format PROPERTY CXX_STANDARD 17)
set_property(TARGET format PROPERTY CXX_STANDARD_REQUIRED True)

# force use of the static msvc runtime so we don't need a bunch of extra dlls
set_property(TARGET format PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

################################################################################

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(format PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(format PRIVATE /W3)
else ()
    message(FATAL_ERROR "could not detect compiler")
endif ()
