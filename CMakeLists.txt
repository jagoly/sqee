project(SQEE)
cmake_minimum_required(VERSION 3.0)

################################################################################

option(SQEE_LINUX "Build for Linux" FALSE)
option(SQEE_WINDOWS "Build for Windows" FALSE)
option(SQEE_ANDROID "Build for Android" FALSE) # TODO
option(SQEE_MACOSX "Build for MacOSX" FALSE) # TODO

if (NOT SQEE_LINUX AND NOT SQEE_MACOSX AND NOT SQEE_WINDOWS AND NOT SQEE_ANDROID)
    message(FATAL_ERROR "Must specify SQEE_[PLATFORM] (LINUX, WINDOWS, ANDROID, MACOSX)")
endif ()

################################################################################

option(SQEE_BUNDLED_LIBS "Use libs and headers from extlibs directory" FALSE)
option(SQEE_STATIC_LINK "Link dependencies to SQEE statically" FALSE)
option(SQEE_STATIC_LIB "Build SQEE as a static library" FALSE)

################################################################################

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DSQEE_DEBUG)
endif ()

if (SQEE_LINUX)
    add_definitions(-DSQEE_LINUX)
elseif (SQEE_WINDOWS)
    add_definitions(-DSQEE_WINDOWS)
elseif (SQEE_ANDROID)
    add_definitions(-DSQEE_ANDROID)
elseif (SQEE_MACOSX)
    add_definitions(-DSQEE_MACOSX)
endif ()

add_definitions(-DRP3D_ENABLE_SWITCH_YZ)

################################################################################

if (CMAKE_COMPILER_IS_GNUCC)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wno-comment ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-strict-aliasing ")

    if (SQEE_LINUX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    elseif (SQEE_WINDOWS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -Wa,-mbig-obj")
    elseif (SQEE_ANDROID)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    elseif (SQEE_MACOSX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif ()

else ()

    message(FATAL_ERROR "Non-GNU compilers are not yet supported, please help fix this :)")

endif ()

################################################################################

set(SRCDIR "${PROJECT_SOURCE_DIR}/source/sqee")
set(INCDIR "${PROJECT_SOURCE_DIR}/include/sqee")

set(SOURCES

  # 3rd Party Source

  "${INCDIR}/redist/lmccop.hpp"
  "${INCDIR}/redist/tinyformat.hpp"
  "${INCDIR}/redist/json.hpp"

  "${INCDIR}/redist/gl_ext_4_2.hpp"
  "${SRCDIR}/redist/gl_ext_4_2.cpp"

  "${INCDIR}/redist/stb_image.hpp"
  "${SRCDIR}/redist/stb_image.cpp"

  # SQEE Source

  "${INCDIR}/setup.hpp"
  "${INCDIR}/builtins.hpp"
  "${INCDIR}/messages.hpp"
  "${INCDIR}/assert.hpp"

  "${INCDIR}/debug/Logging.hpp"           "${SRCDIR}/debug/Logging.cpp"
  "${INCDIR}/debug/Misc.hpp"              "${SRCDIR}/debug/Misc.cpp"
  "${INCDIR}/debug/OpenGL.hpp"            "${SRCDIR}/debug/OpenGL.cpp"
  "${INCDIR}/debug/Text.hpp"              "${SRCDIR}/debug/Text.cpp"

  "${INCDIR}/gl/Context.hpp"              "${SRCDIR}/gl/Context.cpp"
  "${INCDIR}/gl/Drawing.hpp"              "${SRCDIR}/gl/Drawing.cpp"
  "${INCDIR}/gl/FixedBuffer.hpp"          "${SRCDIR}/gl/FixedBuffer.cpp"
  "${INCDIR}/gl/FrameBuffer.hpp"          "${SRCDIR}/gl/FrameBuffer.cpp"
  "${INCDIR}/gl/Shaders.hpp"              "${SRCDIR}/gl/Shaders.cpp"
  "${INCDIR}/gl/TextureBase.hpp"          "${SRCDIR}/gl/TextureBase.cpp"
  "${INCDIR}/gl/Textures.hpp"             "${SRCDIR}/gl/Textures.cpp"
  "${INCDIR}/gl/UniformBuffer.hpp"        "${SRCDIR}/gl/UniformBuffer.cpp"
  "${INCDIR}/gl/VertexArray.hpp"          "${SRCDIR}/gl/VertexArray.cpp"

  "${INCDIR}/scripts/ChaiEngine.hpp"      "${SRCDIR}/scripts/ChaiEngine.cpp"
  "${INCDIR}/scripts/BasicSetup.hpp"      "${SRCDIR}/scripts/BasicSetup.cpp"
  "${INCDIR}/scripts/Helpers.hpp"         "${SRCDIR}/scripts/Helpers.cpp"

  "${INCDIR}/app/Application.hpp"         "${SRCDIR}/app/Application.cpp"
  "${INCDIR}/app/ChaiConsole.hpp"         "${SRCDIR}/app/ChaiConsole.cpp"
  "${INCDIR}/app/DebugOverlay.hpp"        "${SRCDIR}/app/DebugOverlay.cpp"
  "${INCDIR}/app/MessageBus.hpp"          "${SRCDIR}/app/MessageBus.cpp"
  "${INCDIR}/app/PreProcessor.hpp"        "${SRCDIR}/app/PreProcessor.cpp"
  "${INCDIR}/app/Scene.hpp"               "${SRCDIR}/app/Scene.cpp"

  "${INCDIR}/maths/Scalar.hpp"
  "${INCDIR}/maths/Vectors.hpp"
  "${INCDIR}/maths/Matrices.hpp"
  "${INCDIR}/maths/Quaternion.hpp"
  "${INCDIR}/maths/Functions.hpp"
  "${INCDIR}/maths/Volumes.hpp"

  "${INCDIR}/render/Armature.hpp"         "${SRCDIR}/render/Armature.cpp"
  "${INCDIR}/render/Camera.hpp"           "${SRCDIR}/render/Camera.cpp"
  "${INCDIR}/render/Mesh.hpp"             "${SRCDIR}/render/Mesh.cpp"
  "${INCDIR}/render/Material.hpp"         "${SRCDIR}/render/Material.cpp"
  "${INCDIR}/render/Volume.hpp"           "${SRCDIR}/render/Volume.cpp"

  "${INCDIR}/physics/VecTypes.hpp"
  "${INCDIR}/physics/Bodies.hpp"          "${SRCDIR}/physics/Bodies.cpp"

  "${INCDIR}/misc/Algorithms.hpp"
  "${INCDIR}/misc/Resource.hpp"
  "${INCDIR}/misc/StringCast.hpp"         "${SRCDIR}/misc/StringCast.cpp"
  "${INCDIR}/misc/Files.hpp"              "${SRCDIR}/misc/Files.cpp"
  "${INCDIR}/misc/Json.hpp"               "${SRCDIR}/misc/Json.cpp"

  "${INCDIR}/sound/Sound.hpp"             "${SRCDIR}/sound/Sound.cpp"

  "${INCDIR}/dop/Classes.hpp"   "${SRCDIR}/dop/Classes.cpp"
  "${INCDIR}/dop/Functions.hpp" "${SRCDIR}/dop/Functions.cpp"

  # Compiled-In Data

  "${SRCDIR}/data/GLSLHeaders.cpp"
  "${SRCDIR}/data/TextFont.c"
  "${SRCDIR}/data/TextGlow.c"
  "${SRCDIR}/data/Volumes.c"

)

################################################################################

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

################################################################################

include_directories("${PROJECT_SOURCE_DIR}/include")

if (SQEE_BUNDLED_LIBS)
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/")
endif ()

################################################################################

if (SQEE_STATIC_LIB)
    add_library(sqee STATIC ${SOURCES})
else ()
    add_library(sqee SHARED ${SOURCES})
endif ()

################################################################################

if (SQEE_BUNDLED_LIBS)

    if (SQEE_ANDROID)
        set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/android/")
    elseif (SQEE_MACOSX)
        set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/macosx/")
    endif ()

    if (SQEE_STATIC_LINK)
        if (SQEE_LINUX)
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/linux/static/")
        elseif (SQEE_WINDOWS)
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/windows/static/")
        endif ()
    else ()
        if (SQEE_LINUX)
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/linux/dynamic/")
        elseif (SQEE_WINDOWS)
            set(EXTLIBS "${CMAKE_SOURCE_DIR}/extlibs/libs/windows/dynamic/")
        endif ()
    endif ()

    list(APPEND CMAKE_FIND_ROOT_PATH "${EXTLIBS}" "${EXTLIBS}/..")

endif()

################################################################################

find_library(SFML_AUDIO_LIB sfml-audio /)
find_library(SFML_WINDOW_LIB sfml-window /)
find_library(SFML_SYSTEM_LIB sfml-system /)

#find_library(REACTPHYSICS3D_LIB reactphysics3d /)

find_library(OPENGL_LIB NAMES GL opengl32 PATHS /)
find_library(OPENAL_LIB NAMES openal openal32 PATHS /)

################################################################################

target_link_libraries(sqee
    ${SFML_AUDIO_LIB} ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB}
    #${REACTPHYSICS3D_LIB}
    ${OPENGL_LIB} ${OPENAL_LIB}
)

if (SQEE_LINUX)
    target_link_libraries(sqee dl)
elseif (SQEE_WINDOWS)
    target_link_libraries(sqee)
endif ()

################################################################################

if (SQEE_STATIC_LINK)

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

    if (SQEE_LINUX)
        target_link_libraries(sqee X11 X11-xcb xcb xcb-randr xcb-image udev pthread)
    elseif (SQEE_WINDOWS)
        target_link_libraries(sqee winmm gdi32)
    endif ()

endif ()

################################################################################

add_subdirectory("testing")

add_subdirectory("tests/test")
add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")
