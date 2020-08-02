cmake_minimum_required(VERSION 3.12)
project(Format CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.cc")

if (SQEE_STATIC_LIB)
    add_library(format STATIC ${HEADERS} ${SOURCES})
    target_compile_definitions(format PRIVATE SQEE_STATIC_LIB)
else ()
    add_library(format SHARED ${HEADERS} ${SOURCES})
    target_compile_definitions(format PRIVATE SQEE_EXPORT_LIB)
endif ()

target_include_directories(format PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_property(TARGET format PROPERTY CXX_STANDARD 17)
set_property(TARGET format PROPERTY CXX_STANDARD_REQUIRED True)

################################################################################

if (SQEE_LINUX)
    target_compile_definitions(format PRIVATE SQEE_LINUX)
elseif (SQEE_WINDOWS)
    target_compile_definitions(format PRIVATE SQEE_WINDOWS)
else ()
    message(FATAL_ERROR "could not detect platform")
endif ()

if (SQEE_GNU)
    target_compile_definitions(format PRIVATE SQEE_GNU)
elseif (SQEE_CLANG)
    target_compile_definitions(format PRIVATE SQEE_CLANG)
elseif (SQEE_MSVC)
    target_compile_definitions(format PRIVATE SQEE_MSVC)
else ()
    message(FATAL_ERROR "could not detect compiler")
endif ()

################################################################################

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(format PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(format PRIVATE /W3)
endif ()
