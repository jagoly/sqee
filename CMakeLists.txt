cmake_minimum_required(VERSION 3.10)
project(SQEE)

################################################################################

option(SQEE_STATIC_LINK "Link dependencies to SQEE statically" False)
option(SQEE_STATIC_LIB "Build SQEE as a static library" False)

################################################################################

set(CMAKE_MODULE_PATH APPEND "${PROJECT_SOURCE_DIR}/cmake/modules")

################################################################################

include("cmake/sources.cmake")

if (SQEE_STATIC_LIB)

    add_library(sqee STATIC ${SOURCES})
    target_compile_definitions(sqee PUBLIC SQEE_STATIC_LIB)

else ()

    add_library(sqee SHARED ${SOURCES})
    target_compile_definitions(sqee PRIVATE SQEE_EXPORT_LIB)

endif ()

target_include_directories(sqee PUBLIC "${PROJECT_SOURCE_DIR}/include")

################################################################################

set_property(TARGET sqee PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee PROPERTY CXX_STANDARD_REQUIRED True)

set_property(TARGET sqee PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_property(TARGET sqee PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

################################################################################

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(sqee PUBLIC SQEE_DEBUG)
endif ()

################################################################################

if (CMAKE_SYSTEM_NAME MATCHES "Linux")

    set(SQEE_LINUX True)
    target_compile_definitions(sqee PUBLIC SQEE_LINUX)

elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")

    set(SQEE_WINDOWS True)
    target_compile_definitions(sqee PUBLIC SQEE_WINDOWS)

endif ()

################################################################################

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    set(SQEE_GNU True)
    target_compile_definitions(sqee PUBLIC SQEE_GNU)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    set(SQEE_CLANG True)
    target_compile_definitions(sqee PUBLIC SQEE_CLANG)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

    set(SQEE_MSVC True)
    target_compile_definitions(sqee PUBLIC SQEE_MSVC)

endif ()

################################################################################

if (SQEE_GNU OR SQEE_CLANG)

    target_compile_options(sqee PUBLIC -Wall -Wextra)
    #target_compile_options(sqee PRIVATE -Wno-unused-parameter -Wno-strict-aliasing)
    #target_compile_options(sqee PRIVATE -Wno-comment -Wno-misleading-indentation)

    if (SQEE_WINDOWS)
        # todo: actually test this
        target_compile_options(sqee PRIVATE -mwindows -Wa,-mbig-obj)
    endif ()

    if (NOT SQEE_STATIC_LIB)
        target_compile_options(sqee PRIVATE -fvisibility=hidden)
    endif ()

elseif (SQEE_MSVC)

    target_compile_options(sqee PRIVATE /bigobj /W3)
    target_compile_options(sqee PRIVATE /wd4201)

endif ()

################################################################################

if (SQEE_STATIC_LINK)
    set(SFML_STATIC_LIBRARIES True)
endif ()

if (SQEE_WINDOWS AND NOT SFML_ROOT)
    set(SFML_ROOT "${PROJECT_SOURCE_DIR}/extlibs/sfml")
endif ()

find_package(SFML 2.4 COMPONENTS audio window system REQUIRED)

target_include_directories(sqee PRIVATE ${SFML_INCLUDE_DIR})

# need to refactor sq::Sound so that I can keep SFML internal
target_link_libraries(sqee PUBLIC ${SFML_LIBRARIES})
target_link_libraries(sqee PRIVATE ${SFML_DEPENDENCIES})

################################################################################

find_package(OpenGL REQUIRED)

target_link_libraries(sqee PUBLIC ${OPENGL_gl_LIBRARY})

################################################################################

export(TARGETS sqee FILE sqee-exports.cmake)

################################################################################

add_subdirectory("testing")

add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")
