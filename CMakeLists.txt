cmake_minimum_required(VERSION 3.12)
project(SQEE CXX)

################################################################################

option(SQEE_STATIC_LINK "Link dependencies to SQEE statically" False)
option(SQEE_STATIC_LIB "Build SQEE as a static library" False)

################################################################################

set(CMAKE_MODULE_PATH APPEND "${PROJECT_SOURCE_DIR}/cmake/modules")

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/sqee/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/sqee/*.cpp")

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

    target_compile_options(sqee PRIVATE -Wall -Wextra)

    if (NOT SQEE_STATIC_LIB)
        target_compile_options(sqee PRIVATE -fvisibility=hidden)
    endif ()

elseif (SQEE_MSVC)

    target_compile_options(sqee PRIVATE /bigobj)
    target_compile_options(sqee PRIVATE /W3 /wd4201)

endif ()

################################################################################

if (SQEE_STATIC_LINK)
    set(SFML_STATIC_LIBRARIES True)
endif ()

find_package(SFML 2.5 COMPONENTS audio window system REQUIRED)

# need to refactor sq::Sound so that I can make SFML private

target_include_directories(sqee PUBLIC ${SFML_INCLUDE_DIR})
target_link_libraries(sqee PUBLIC sfml-audio sfml-window sfml-system)

################################################################################

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(OpenGL REQUIRED)

if (SQEE_LINUX)
    target_link_libraries(sqee PUBLIC GL GLX)
elseif (SQEE_WINDOWS)
    target_link_libraries(sqee PUBLIC opengl32)
endif ()

################################################################################

if (SQEE_STATIC_LINK)
    set(LUA54CPP_STATIC_LIB True)
endif ()

add_subdirectory(lua54cpp)

target_include_directories(sqee PUBLIC "lua54cpp/src")
target_link_libraries(sqee PUBLIC lua54cpp)

target_compile_definitions(sqee PUBLIC SOL_ALL_SAFETIES_ON=1)
target_compile_definitions(sqee PUBLIC SOL_USING_CXX_LUA=1)
target_compile_definitions(sqee PUBLIC SOL_PRINT_ERRORS=0)

################################################################################

export(TARGETS sqee lua54cpp FILE sqee-exports.cmake)

################################################################################

add_subdirectory("testing")

add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")
