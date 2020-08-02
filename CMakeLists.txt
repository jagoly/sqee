cmake_minimum_required(VERSION 3.12)
project(SQEE CXX)

################################################################################

option(SQEE_STATIC_LIB "Build SQEE as a static library" False)

################################################################################

set(CMAKE_MODULE_PATH APPEND "${PROJECT_SOURCE_DIR}/cmake/modules")

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

# todo: makes these options so you don't have to change this file to enable them

#set_property(TARGET sqee PROPERTY CXX_INCLUDE_WHAT_YOU_USE
#    "iwyu;-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;--mapping_file=${PROJECT_SOURCE_DIR}/extras/iwyu-mapping.imp")
#target_include_directories(sqee SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/include")

#target_precompile_headers(sqee PUBLIC <sqee/setup.hpp>)

###############################################################################

set_property(TARGET sqee PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee PROPERTY CXX_STANDARD_REQUIRED True)

################################################################################

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(sqee PUBLIC SQEE_DEBUG)
endif ()

################################################################################

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    set(SQEE_LINUX True)
    target_compile_definitions(sqee PUBLIC SQEE_LINUX)

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

    if (NOT SQEE_STATIC_LIB)
        target_compile_options(sqee PRIVATE -fvisibility=hidden)
    endif ()

elseif (SQEE_MSVC)

    target_compile_options(sqee PRIVATE /bigobj)
    target_compile_options(sqee PRIVATE /W3 /wd4251)

endif ()

################################################################################

if (SQEE_STATIC_LIB)
    set(SFML_STATIC_LIBRARIES True)
endif ()

find_package(SFML 2.5 COMPONENTS audio window system REQUIRED)

target_include_directories(sqee PRIVATE ${SFML_INCLUDE_DIR})
target_link_libraries(sqee PRIVATE sfml-audio sfml-window sfml-system)

################################################################################

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(OpenGL REQUIRED)

if (SQEE_LINUX)
    target_link_libraries(sqee PUBLIC GL GLX)
elseif (SQEE_WINDOWS)
    target_link_libraries(sqee PUBLIC opengl32)
endif ()

################################################################################

add_subdirectory(libs/dearimgui)
target_link_libraries(sqee PUBLIC dearimgui)

################################################################################

add_subdirectory(libs/format)
target_link_libraries(sqee PUBLIC format)

################################################################################

add_subdirectory(libs/lua54cpp)
target_link_libraries(sqee PUBLIC lua54cpp)

target_compile_definitions(sqee PUBLIC SOL_ALL_SAFETIES_ON=1)
target_compile_definitions(sqee PUBLIC SOL_USING_CXX_LUA=1)
target_compile_definitions(sqee PUBLIC SOL_PRINT_ERRORS=0)

################################################################################

export(TARGETS sqee dearimgui format lua54cpp FILE sqee-exports.cmake)

################################################################################

add_subdirectory("testing")

add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")
