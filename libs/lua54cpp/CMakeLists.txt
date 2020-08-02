cmake_minimum_required(VERSION 3.12)
project(Lua54CPP CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.cpp")

if (SQEE_STATIC_LIB)
    add_library(lua54cpp STATIC ${HEADERS} ${SOURCES})
    target_compile_definitions(lua54cpp PRIVATE SQEE_STATIC_LIB)
else ()
    add_library(lua54cpp SHARED ${HEADERS} ${SOURCES})
    target_compile_definitions(lua54cpp PRIVATE SQEE_EXPORT_LIB)
endif ()

target_include_directories(lua54cpp PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_property(TARGET lua54cpp PROPERTY CXX_STANDARD 17)
set_property(TARGET lua54cpp PROPERTY CXX_STANDARD_REQUIRED True)

################################################################################

if (SQEE_LINUX)
    target_compile_definitions(lua54cpp PRIVATE SQEE_LINUX)
elseif (SQEE_WINDOWS)
    target_compile_definitions(lua54cpp PRIVATE SQEE_WINDOWS)
else ()
    message(FATAL_ERROR "could not detect platform")
endif ()

if (SQEE_GNU)
    target_compile_definitions(lua54cpp PRIVATE SQEE_GNU)
elseif (SQEE_CLANG)
    target_compile_definitions(lua54cpp PRIVATE SQEE_CLANG)
elseif (SQEE_MSVC)
    target_compile_definitions(lua54cpp PRIVATE SQEE_MSVC)
else ()
    message(FATAL_ERROR "could not detect compiler")
endif ()

################################################################################

# note that lua needs relaxed warning settings compared to sqee

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(lua54cpp PRIVATE -Wall)
elseif (SQEE_MSVC)
    target_compile_options(lua54cpp PRIVATE /W2)
endif ()
