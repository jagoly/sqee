project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(SQEE_USE_BUNDLED_LIBS FALSE CACHE BOOLEAN "Use libraries and headers in the extlibs dir")
set(SQEE_STATIC_LIB FALSE CACHE BOOLEAN "Build SQEE as a static library")

set(SQEE_LINUX FALSE CACHE BOOLEAN "Build for Linux")
set(SQEE_WINOWS FALSE CACHE BOOLEAN "Build for Windows")
set(SQEE_ANDROID FALSE CACHE BOOLEAN "Build for Android")
# android not currently implemented

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-comment -Wno-strict-aliasing")
if(${SQEE_WINDOWS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -static-libstdc++ -static-libgcc")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSQEE_DEBUG -O1")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

set(SRCDIR "${PROJECT_SOURCE_DIR}/src/sqee")
set(INCDIR "${PROJECT_SOURCE_DIR}/include/sqee")
set(EMBDIR "${PROJECT_SOURCE_DIR}/embed")

set(SOURCES
# 3rd Party Source
    "${INCDIR}/redist/lmccop.hpp"
    "${INCDIR}/redist/tinyformat.hpp"
    "${INCDIR}/redist/gl_ext_3_3.hpp"       "${SRCDIR}/redist/gl_ext_3_3.cpp"
    "${INCDIR}/redist/stb_image.hpp"        "${SRCDIR}/redist/stb_image.cpp"
    "${INCDIR}/redist/jsoncpp.hpp"          "${SRCDIR}/redist/jsoncpp.cpp"

# SQEE Source
    "${INCDIR}/forward.hpp"
    "${INCDIR}/debug/Callbacks.hpp"
    "${INCDIR}/debug/Misc.hpp"              "${SRCDIR}/debug/Misc.cpp"
    "${INCDIR}/gl/Drawing.hpp"              "${SRCDIR}/gl/Drawing.cpp"
    "${INCDIR}/gl/Shaders.hpp"              "${SRCDIR}/gl/Shaders.cpp"
    "${INCDIR}/gl/Textures.hpp"             "${SRCDIR}/gl/Textures.cpp"
    "${INCDIR}/gl/FrameBuffer.hpp"          "${SRCDIR}/gl/FrameBuffer.cpp"
    "${INCDIR}/gl/UniformBuffer.hpp"        "${SRCDIR}/gl/UniformBuffer.cpp"
    "${INCDIR}/scripts/ChaiScript.hpp"      "${SRCDIR}/scripts/ChaiScript.cpp"
    "${INCDIR}/scripts/ConsoleScene.hpp"    "${SRCDIR}/scripts/ConsoleScene.cpp"
    "${INCDIR}/scripts/ConsoleHandler.hpp"  "${SRCDIR}/scripts/ConsoleHandler.cpp"
    "${INCDIR}/scripts/Intergration.hpp"    "${SRCDIR}/scripts/Intergration.cpp"
    "${INCDIR}/app/Logging.hpp"             "${SRCDIR}/app/Logging.cpp"
    "${INCDIR}/app/Application.hpp"         "${SRCDIR}/app/Application.cpp"
    "${INCDIR}/app/SettingsMaps.hpp"        "${SRCDIR}/app/SettingsMaps.cpp"
    "${INCDIR}/app/PreProcessor.hpp"        "${SRCDIR}/app/PreProcessor.cpp"
    "${INCDIR}/handlers/Handler.hpp"        "${SRCDIR}/handlers/Handler.cpp"
    "${INCDIR}/handlers/Basics.hpp"         "${SRCDIR}/handlers/Basics.cpp"
    "${INCDIR}/text/Font.hpp"               "${SRCDIR}/text/Font.cpp"
    "${INCDIR}/text/Text.hpp"               "${SRCDIR}/text/Text.cpp"
    "${INCDIR}/maths/Culling.hpp"           "${SRCDIR}/maths/Culling.cpp"
    "${INCDIR}/maths/General.hpp"           "${SRCDIR}/maths/General.cpp"
    "${INCDIR}/maths/Physics.hpp"           "${SRCDIR}/maths/Physics.cpp"
    "${INCDIR}/menus/BasicMenu.hpp"         "${SRCDIR}/menus/BasicMenu.cpp"
    "${INCDIR}/render/Armature.hpp"         "${SRCDIR}/render/Armature.cpp"
    "${INCDIR}/render/Camera.hpp"           "${SRCDIR}/render/Camera.cpp"
    "${INCDIR}/render/Mesh.hpp"             "${SRCDIR}/render/Mesh.cpp"
    "${INCDIR}/render/Skin.hpp"             "${SRCDIR}/render/Skin.cpp"
    "${INCDIR}/physics/RP3D.hpp"            "${SRCDIR}/physics/RP3D.cpp"
    "${INCDIR}/physics/PhysObject.hpp"      "${SRCDIR}/physics/PhysObject.cpp"
    "${INCDIR}/misc/ResHolder.hpp"          "${SRCDIR}/misc/ResHolder.cpp"
    "${INCDIR}/misc/Files.hpp"              "${SRCDIR}/misc/Files.cpp"
    "${INCDIR}/scenes/Scene.hpp"            "${SRCDIR}/scenes/Scene.cpp"
    "${INCDIR}/scenes/Basics.hpp"           "${SRCDIR}/scenes/Basics.cpp"
    "${INCDIR}/sounds/SoundManager.hpp"     "${SRCDIR}/sounds/SoundManager.cpp"

# Compiled-In Data
    "${SRCDIR}/data/GLSLHeaders.c"
    "${SRCDIR}/data/TextFont.c"
    "${SRCDIR}/data/TextGlow.c"
    "${SRCDIR}/data/Volumes.c"
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

include_directories("${PROJECT_SOURCE_DIR}/include")
if(${SQEE_USE_BUNDLED_LIBS})
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/")
endif()

if(${SQEE_STATIC_LIB})
    add_library(sqee STATIC ${SOURCES} ${EMBEDSOURCES})
else()
    add_library(sqee SHARED ${SOURCES} ${EMBEDSOURCES})
endif()

if(${SQEE_USE_BUNDLED_LIBS})
    if(${SQEE_LINUX})
        list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/extlibs/libs/lin64)
    elseif(${SQEE_WINDOWS})
        list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/extlibs/libs/win64)
    endif()
endif()


find_library(SFML_WINDOW_LIB sfml-window /)
find_library(SFML_SYSTEM_LIB sfml-system /)
find_library(SFML_AUDIO_LIB sfml-audio /)
find_library(REACTPHYSICS3D_LIB reactphysics3d /)
find_library(OPENGL_LIB NAMES GL opengl32 PATHS /)
find_library(OPENAL_LIB NAMES openal openal32 PATHS /)

target_link_libraries(sqee
    ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${SFML_AUDIO_LIB}
    ${REACTPHYSICS3D_LIB} ${OPENGL_LIB} ${OPENAL_LIB}
)

add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")
