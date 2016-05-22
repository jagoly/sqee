project(SQEE)
cmake_minimum_required(VERSION 2.8)

option(SQEE_LINUX "Build for Linux" FALSE)
option(SQEE_WINDOWS "Build for Windows" FALSE)
option(SQEE_ANDROID "Build for Android" FALSE) # TODO
option(SQEE_MACOSX "Build for MacOSX" FALSE) # TODO

option(SQEE_SPECIAL_LINUX_DEBUG "Special mode for using debug libs on linux" FALSE)
option(SQEE_BUNDLED_LIBS "Use libs and headers from extlibs directory" TRUE)
option(SQEE_STATIC_LINK "Link dependencies to SQEE statically" FALSE)
option(SQEE_STATIC_LIB "Build SQEE as a static library" FALSE)
option(SQEE_DEBUG "Enable some SQEE debug features" TRUE)

if(NOT SQEE_LINUX AND NOT SQEE_MACOSX AND NOT SQEE_WINDOWS AND NOT SQEE_ANDROID)
    message(FATAL_ERROR "Must specify SQEE_[PLATFORM] (LINUX, WINDOWS, ANDROID, MACOSX)")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(SQEE_DEBUG)
    add_definitions(-DSQEE_DEBUG)
endif()

if(SQEE_LINUX)
    add_definitions(-DSQEE_LINUX)
elseif(SQEE_WINDOWS)
    add_definitions(-DSQEE_WINDOWS)
elseif(SQEE_ANDROID)
    add_definitions(-DSQEE_ANDROID)
elseif(SQEE_MACOSX)
    add_definitions(-DSQEE_MACOSX)
endif()


if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wno-comment")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-strict-aliasing")

    if(SQEE_LINUX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    elseif(SQEE_WINDOWS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -Wa,-mbig-obj")
    elseif(SQEE_ANDROID)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    elseif(SQEE_MACOSX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
else()
    message(FATAL_ERROR "Non-GNU compilers are not yet supported, please help fix this :)")
endif()


set(SRCDIR "${PROJECT_SOURCE_DIR}/source/sqee")
set(INCDIR "${PROJECT_SOURCE_DIR}/include/sqee")

set(SOURCES
# 3rd Party Source
    "${INCDIR}/redist/lmccop.hpp"
    "${INCDIR}/redist/nanoflann.hpp"
    "${INCDIR}/redist/tinyformat.hpp"
    "${INCDIR}/redist/gl_ext_4_2.hpp"       "${SRCDIR}/redist/gl_ext_4_2.cpp"
    "${INCDIR}/redist/stb_image.hpp"        "${SRCDIR}/redist/stb_image.cpp"

# SQEE Source
    "${INCDIR}/setup.hpp"                   "${INCDIR}/assert.hpp"
    "${INCDIR}/forward.hpp"                 "${INCDIR}/builtins.hpp"
    "${INCDIR}/debug/Misc.hpp"              "${SRCDIR}/debug/Misc.cpp"
    "${INCDIR}/debug/Logging.hpp"           "${SRCDIR}/debug/Logging.cpp"
    "${INCDIR}/debug/OpenGL.hpp"            "${SRCDIR}/debug/OpenGL.cpp"
    "${INCDIR}/gl/Drawing.hpp"              "${SRCDIR}/gl/Drawing.cpp"
    "${INCDIR}/gl/Shaders.hpp"              "${SRCDIR}/gl/Shaders.cpp"
    "${INCDIR}/gl/Textures.hpp"             "${SRCDIR}/gl/Textures.cpp"
    "${INCDIR}/gl/FrameBuffer.hpp"          "${SRCDIR}/gl/FrameBuffer.cpp"
    "${INCDIR}/gl/VertexArray.hpp"          "${SRCDIR}/gl/VertexArray.cpp"
    "${INCDIR}/gl/FixedBuffer.hpp"          "${SRCDIR}/gl/FixedBuffer.cpp"
    "${INCDIR}/gl/DynamicBuffer.hpp"        "${SRCDIR}/gl/DynamicBuffer.cpp"
    "${INCDIR}/gl/UniformBuffer.hpp"        "${SRCDIR}/gl/UniformBuffer.cpp"
    "${INCDIR}/scripts/ChaiScript.hpp"      "${SRCDIR}/scripts/ChaiScript.cpp"
    "${INCDIR}/scripts/BasicSetup.hpp"      "${SRCDIR}/scripts/BasicSetup.cpp"
    "${INCDIR}/ecs/Component.hpp"           "${SRCDIR}/ecs/Component.cpp"
    "${INCDIR}/ecs/Entity.hpp"              "${SRCDIR}/ecs/Entity.cpp"
    "${INCDIR}/app/Settings.hpp"            "${SRCDIR}/app/Settings.cpp"
    "${INCDIR}/app/Resources.hpp"           "${SRCDIR}/app/Resources.cpp"
    "${INCDIR}/app/Application.hpp"         "${SRCDIR}/app/Application.cpp"
    "${INCDIR}/app/PreProcessor.hpp"        "${SRCDIR}/app/PreProcessor.cpp"
    "${INCDIR}/app/DebugOverlay.hpp"        "${SRCDIR}/app/DebugOverlay.cpp"
    "${INCDIR}/app/ChaiConsole.hpp"         "${SRCDIR}/app/ChaiConsole.cpp"
    "${INCDIR}/app/MessageBus.hpp"          "${SRCDIR}/app/MessageBus.cpp"
    "${INCDIR}/text/Font.hpp"               "${SRCDIR}/text/Font.cpp"
    "${INCDIR}/text/Text.hpp"               "${SRCDIR}/text/Text.cpp"
    "${INCDIR}/maths/Scalar.hpp"            "${INCDIR}/maths/Vectors.hpp"
    "${INCDIR}/maths/Matrices.hpp"          "${INCDIR}/maths/Quaternion.hpp"
    "${INCDIR}/maths/Volumes.hpp"           "${SRCDIR}/maths/Volumes.cpp"
    "${INCDIR}/maths/General.hpp"           "${SRCDIR}/maths/General.cpp"
    "${INCDIR}/menus/BasicMenu.hpp"         "${SRCDIR}/menus/BasicMenu.cpp"
    "${INCDIR}/render/Armature.hpp"         "${SRCDIR}/render/Armature.cpp"
    "${INCDIR}/render/Camera.hpp"           "${SRCDIR}/render/Camera.cpp"
    "${INCDIR}/render/Mesh.hpp"             "${SRCDIR}/render/Mesh.cpp"
    "${INCDIR}/render/Skin.hpp"             "${SRCDIR}/render/Skin.cpp"
    "${INCDIR}/physics/VecTypes.hpp"
    "${INCDIR}/physics/PhysObject.hpp"      "${SRCDIR}/physics/PhysObject.cpp"
    "${INCDIR}/misc/StringCast.hpp"         "${SRCDIR}/misc/StringCast.cpp"
    "${INCDIR}/misc/OrderedMap.hpp"         "${INCDIR}/misc/ResHolder.hpp"
    "${INCDIR}/misc/Files.hpp"              "${SRCDIR}/misc/Files.cpp"
    "${INCDIR}/scenes/Scene.hpp"            "${SRCDIR}/scenes/Scene.cpp"
    "${INCDIR}/scenes/Basics.hpp"           "${SRCDIR}/scenes/Basics.cpp"
    "${INCDIR}/sounds/SoundManager.hpp"     "${SRCDIR}/sounds/SoundManager.cpp"

# Compiled-In Data
    "${SRCDIR}/data/GLSLHeaders.cpp"
    "${SRCDIR}/data/TextFont.c"
    "${SRCDIR}/data/TextGlow.c"
    "${SRCDIR}/data/Volumes.c"
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

include_directories("${PROJECT_SOURCE_DIR}/include")
if(SQEE_BUNDLED_LIBS)
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/")
endif()

if(SQEE_STATIC_LIB)
    add_library(sqee STATIC ${SOURCES})
else()
    add_library(sqee SHARED ${SOURCES})
endif()


if(SQEE_BUNDLED_LIBS)
    if(SQEE_ANDROID)
        set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/android/")
    elseif(SQEE_MACOSX)
        set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/macosx/")
    endif()

    if(SQEE_STATIC_LINK)
        if(SQEE_LINUX)
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/linux/static/")
        elseif(SQEE_WINDOWS)
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/windows/static/")
        endif()
    else()
        if(SQEE_LINUX)
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/linux/dynamic/")
        elseif(SQEE_WINDOWS)
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/windows/dynamic/")
        endif()
    endif()

    if(SQEE_SPECIAL_LINUX_DEBUG)
        if(NOT SQEE_LINUX OR SQEE_STATIC_LINK OR NOT CMAKE_BUILD_TYPE MATCHES "Debug")
            message(FATAL_ERROR "Options incorrect for Special Linux Debug Mode")
        else()
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/linux/debug/")
        endif()
    endif()

    list(APPEND CMAKE_FIND_ROOT_PATH "${EXTLIBS}" "${EXTLIBS}/..")
endif()


find_library(SFML_AUDIO_LIB sfml-audio /)
find_library(SFML_WINDOW_LIB sfml-window /)
find_library(SFML_SYSTEM_LIB sfml-system /)

find_library(REACTPHYSICS3D_LIB reactphysics3d /)

find_library(OPENGL_LIB NAMES GL opengl32 PATHS /)
find_library(OPENAL_LIB NAMES openal openal32 PATHS /)


target_link_libraries(sqee
    ${SFML_AUDIO_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB}
    ${REACTPHYSICS3D_LIB} ${OPENGL_LIB} ${OPENAL_LIB}
)


if(SQEE_LINUX)
    target_link_libraries(sqee dl)
elseif(SQEE_WINDOWS)
    target_link_libraries(sqee)
endif()


if(SQEE_STATIC_LINK)
    add_definitions(-DSFML_STATIC)

    find_library(VORBISFILE_LIB vorbisfile /)
    find_library(VORBISENC_LIB vorbisenc /)
    find_library(VORBIS_LIB vorbis /)
    find_library(FLAC_LIB FLAC /)
    find_library(OGG_LIB ogg /)

    target_link_libraries(sqee
        ${VORBISFILE_LIB} ${VORBISENC_LIB}
        ${VORBIS_LIB} ${FLAC_LIB} ${OGG_LIB}
    )

    if(SQEE_LINUX)
        target_link_libraries(sqee X11 X11-xcb xcb xcb-randr xcb-image udev pthread)
    elseif(SQEE_WINDOWS)
        target_link_libraries(sqee winmm gdi32)
    endif()
endif()


add_subdirectory("tests/test")
add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")
