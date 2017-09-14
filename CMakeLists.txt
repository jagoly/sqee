cmake_minimum_required(VERSION 3.4)
project(SQEE)

################################################################################

option(SQEE_STATIC_LINK "Link dependencies to SQEE statically" FALSE)
option(SQEE_STATIC_LIB "Build SQEE as a static library" FALSE)

################################################################################

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

################################################################################

if (CMAKE_SYSTEM_NAME MATCHES "Linux")

    set(SQEE_LINUX True)
    add_definitions(-DSQEE_LINUX)

elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")

    set(SQEE_WINDOWS True)
    add_definitions(-DSQEE_WINDOWS)

endif ()

################################################################################

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    set(SQEE_GNU True)
    add_definitions(-DSQEE_GNU)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    set(SQEE_CLANG True)
    add_definitions(-DSQEE_CLANG)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

    set(SQEE_MSVC True)
    add_definitions(-DSQEE_MSVC)

endif ()

################################################################################

if (SQEE_STATIC_LINK)
    set(SFML_STATIC_LIBRARIES True)
endif ()

if (SQEE_WINDOWS AND NOT SFML_ROOT)
    set(SFML_ROOT "${PROJECT_SOURCE_DIR}/extlibs/sfml")
endif ()

find_package(SFML 2.4 COMPONENTS audio window system REQUIRED)

add_definitions(-DRP3D_ENABLE_SWITCH_YZ)

################################################################################

if (SQEE_STATIC_LIB)
    add_definitions(-DSQEE_STATIC_LIB)
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DSQEE_DEBUG)
endif ()

################################################################################

if (SQEE_GNU OR SQEE_CLANG)

    string(APPEND CMAKE_CXX_FLAGS " -std=c++17 -Wall -Wextra")

    string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-parameter -Wno-strict-aliasing")
    string(APPEND CMAKE_CXX_FLAGS " -Wno-comment -Wno-misleading-indentation")

    if (SQEE_GNU)
        # todo: remove when warnings for structured bindings fixed
        string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-variable")
    endif ()

    if (SQEE_CLANG)
        # todo: remove when no-misleading-indentation becomes supported
        string(APPEND CMAKE_CXX_FLAGS " -Wno-unknown-warning-option")
    endif ()

    if (SQEE_WINDOWS)
        string(APPEND CMAKE_CXX_FLAGS " -mwindows -Wa,-mbig-obj")
    endif ()

#    if (NOT SQEE_STATIC_LIB)
#        string(APPEND CMAKE_CXX_FLAGS " -fvisibility=hidden")
#    endif ()

elseif (SQEE_MSVC)

    string(APPEND CMAKE_CXX_FLAGS " /std:c++17 /bigobj /W3 /wd4201")

    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True)

endif ()

################################################################################

include("cmake/sources.cmake")

if (NOT SQEE_STATIC_LIB)
    add_library(sqee SHARED ${SOURCES})
    target_compile_definitions(sqee PRIVATE SQEE_EXPORT_LIB)
else ()
    add_library(sqee STATIC ${SOURCES})
endif ()

################################################################################

include_directories(${SFML_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(sqee ${SFML_LIBRARIES})
target_link_libraries(sqee ${SFML_DEPENDENCIES})

find_package(OpenGL REQUIRED)
target_link_libraries(sqee ${OPENGL_gl_LIBRARY})

################################################################################

#find_package(ReactPhysics3D REQUIRED)

#include_directories(${RP3D_INCLUDE_DIR})
#target_link_libraries(sqee ${RP3D_LIBRARY})

################################################################################

add_subdirectory("tests")
add_subdirectory("testing")

add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")
