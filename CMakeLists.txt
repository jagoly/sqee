cmake_minimum_required(VERSION 3.16)
project(SQEE CXX)

################################################################################

option(SQEE_STATIC_LIB "Build SQEE as a static library" False)

################################################################################

set(CMAKE_MODULE_PATH APPEND "${PROJECT_SOURCE_DIR}/cmake/modules")
set(CMAKE_INCLUDE_PATH APPEND "${PROJECT_SOURCE_DIR}/cmake")

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.cpp")

if (SQEE_STATIC_LIB)

    add_library(sqee STATIC ${HEADERS} ${SOURCES})
    target_compile_definitions(sqee PUBLIC SQEE_STATIC_LIB)

else ()

    add_library(sqee SHARED ${HEADERS} ${SOURCES})
    target_compile_definitions(sqee PRIVATE SQEE_EXPORT_LIB)

endif ()

target_include_directories(sqee PUBLIC "${PROJECT_SOURCE_DIR}/include")

################################################################################

# todo: makes these options so you don't have to change this file to enable them

#set_property(TARGET sqee PROPERTY CXX_INCLUDE_WHAT_YOU_USE
#    "iwyu;-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;--mapping_file=${PROJECT_SOURCE_DIR}/extras/iwyu-mapping.imp")
#target_include_directories(sqee SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/include")

#target_precompile_headers(sqee PUBLIC <sqee/setup.hpp>)

################################################################################

set_property(TARGET sqee PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee PROPERTY CXX_STANDARD_REQUIRED True)

target_compile_definitions(sqee PUBLIC $<$<CONFIG:Debug>:SQEE_DEBUG>)

# force use of the static msvc runtime so we don't need a bunch of extra dlls
set_property(TARGET sqee PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

################################################################################

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    set(SQEE_LINUX True)
    target_compile_definitions(sqee PUBLIC SQEE_LINUX)

    # required for stack traces from backward.hpp
    #target_link_libraries(sqee PUBLIC $<$<CONFIG:Debug>:dw>)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")

    set(SQEE_WINDOWS True)
    target_compile_definitions(sqee PUBLIC SQEE_WINDOWS)

else ()
    message(FATAL_ERROR "unsupported platform '${CMAKE_SYSTEM_NAME}'")
endif ()

################################################################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    set(SQEE_GNU True)
    target_compile_definitions(sqee PUBLIC SQEE_GNU)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    set(SQEE_CLANG True)
    target_compile_definitions(sqee PUBLIC SQEE_CLANG)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    set(SQEE_MSVC True)
    target_compile_definitions(sqee PUBLIC SQEE_MSVC)

else ()
    message(FATAL_ERROR "unsupported compiler '${CMAKE_CXX_COMPILER_ID}'")
endif ()

################################################################################

if (SQEE_GNU OR SQEE_CLANG)

    target_compile_options(sqee PRIVATE -Wall -Wextra)
    #target_compile_options(sqee PRIVATE -Wall -Wextra -Wpedantic)

    if (NOT SQEE_STATIC_LIB)
        target_compile_options(sqee PRIVATE -fvisibility=hidden)
    endif ()

elseif (SQEE_MSVC)

    target_compile_options(sqee PRIVATE /bigobj)
    target_compile_options(sqee PRIVATE /W3 /wd4251)

endif ()

################################################################################

if (SQEE_STATIC_LIB)
    find_package(Threads REQUIRED)
    target_link_libraries(sqee PUBLIC Threads::Threads)
    target_link_libraries(sqee PUBLIC ${CMAKE_DL_LIBS})
endif ()

################################################################################

# for now, don't bother trying to link sfml statically on linux
if (SQEE_STATIC_LIB AND SQEE_WINDOWS)
    set(SFML_STATIC_LIBRARIES True)
endif ()

find_package(SFML 2.5 COMPONENTS window system REQUIRED)

target_include_directories(sqee PRIVATE ${SFML_INCLUDE_DIR})
target_link_libraries(sqee PRIVATE sfml-window sfml-system)

################################################################################

find_package(glfw3 3.3 REQUIRED)

target_link_libraries(sqee PRIVATE glfw)

################################################################################

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(OpenGL REQUIRED)

if (SQEE_LINUX)
    target_link_libraries(sqee PUBLIC GL GLX)
elseif (SQEE_WINDOWS)
    target_link_libraries(sqee PUBLIC opengl32)
endif ()

################################################################################

message(STATUS "VULKAN_SDK = $ENV{VULKAN_SDK}")

find_package(Vulkan REQUIRED)

target_link_libraries(sqee PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(sqee PUBLIC ${Vulkan_INCLUDE_DIRS})

################################################################################

set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_GLSL OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_MSL OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_REFLECT OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_C_API OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "" FORCE)

if (SQEE_STATIC_LIB)
    set(SPIRV_CROSS_FORCE_PIC OFF CACHE BOOL "" FORCE)
else ()
    set(SPIRV_CROSS_FORCE_PIC ON CACHE BOOL "" FORCE)
endif ()

add_subdirectory(libs/SPIRV-Cross)
set_property(TARGET spirv-cross-core PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
target_link_libraries(sqee PRIVATE spirv-cross-core)

################################################################################

add_subdirectory(libs/dearimgui)
target_link_libraries(sqee PUBLIC dearimgui)

add_subdirectory(libs/format)
target_link_libraries(sqee PUBLIC format)

add_subdirectory(libs/wren)
target_link_libraries(sqee PUBLIC wren)

################################################################################

include(CMakePackageConfigHelpers)

write_basic_package_version_file (
    "${CMAKE_CURRENT_BINARY_DIR}/sqeeConfigVersion.cmake"
    VERSION 1.0
    COMPATIBILITY AnyNewerVersion
)

export (
    TARGETS sqee dearimgui format wren
    FILE "${CMAKE_CURRENT_BINARY_DIR}/sqee-exports.cmake"
)

configure_file (
    cmake/sqeeConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/sqeeConfig.cmake"
)

add_subdirectory("testing")

add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")

add_subdirectory("tests/vkdemo")
