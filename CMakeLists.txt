cmake_minimum_required(VERSION 3.2)
project(SQEE)

################################################################################

option(SQEE_LINUX "Build for Linux" FALSE)
option(SQEE_WINDOWS "Build for Windows" FALSE)
option(SQEE_ANDROID "Build for Android" FALSE) # TODO
option(SQEE_MACOSX "Build for MacOSX" FALSE) # TODO

if (NOT SQEE_LINUX AND NOT SQEE_MACOSX AND NOT SQEE_WINDOWS AND NOT SQEE_ANDROID)
    message(FATAL_ERROR "Must specify SQEE_[PLATFORM] (LINUX, WINDOWS, ANDROID, MACOSX)")
endif ()

################################################################################

option(SQEE_STATIC_LINK "Link dependencies to SQEE statically" FALSE)
option(SQEE_STATIC_LIB "Build SQEE as a static library" FALSE)

################################################################################

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DSQEE_DEBUG)
endif ()

################################################################################

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

string(APPEND CMAKE_CXX_FLAGS " -std=c++17")

################################################################################

string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-parameter -Wno-strict-aliasing")
string(APPEND CMAKE_CXX_FLAGS " -Wno-comment -Wno-misleading-indentation")

if (SQEE_LINUX)
    string(APPEND CMAKE_CXX_FLAGS "")
elseif (SQEE_WINDOWS)
    string(APPEND CMAKE_CXX_FLAGS " -mwindows -Wa,-mbig-obj")
elseif (SQEE_ANDROID)
    string(APPEND CMAKE_CXX_FLAGS "")
elseif (SQEE_MACOSX)
    string(APPEND CMAKE_CXX_FLAGS "")
endif ()

################################################################################

set(SOURCES

  #==========================================================#
  # 3rd Party Source                                         #
  #==========================================================#

  "include/sqee/redist/gl_loader.hpp"
  "source/sqee/redist/gl_loader.cpp"

  "include/sqee/redist/stb_image.hpp"
  "source/sqee/redist/stb_image.cpp"

  "include/sqee/redist/lmccop.hpp"
  "include/sqee/redist/nl_json.hpp"
  "include/sqee/redist/tinyformat.hpp"


  #==========================================================#
  # SQEE Source                                              #
  #==========================================================#

  "include/sqee/assert.hpp"
  "include/sqee/builtins.hpp"
  "include/sqee/helpers.hpp"
  "include/sqee/macros.hpp"
  "include/sqee/setup.hpp"

  #----------------------------------------------------------#

  "include/sqee/app/Application.hpp"   "source/sqee/app/Application.cpp"
  "include/sqee/app/ChaiConsole.hpp"   "source/sqee/app/ChaiConsole.cpp"
  "include/sqee/app/ChaiEngine.hpp"    "source/sqee/app/ChaiEngine.cpp"
  "include/sqee/app/DebugOverlay.hpp"  "source/sqee/app/DebugOverlay.cpp"
  "include/sqee/app/InputDevices.hpp"  "source/sqee/app/InputDevices.cpp"
  "include/sqee/app/MessageBus.hpp"    "source/sqee/app/MessageBus.cpp"
  "include/sqee/app/PreProcessor.hpp"  "source/sqee/app/PreProcessor.cpp"
  "include/sqee/app/Scene.hpp"         "source/sqee/app/Scene.cpp"
  "include/sqee/app/Window.hpp"        "source/sqee/app/Window.cpp"

  "include/sqee/app/Event.hpp"

  #----------------------------------------------------------#

  "include/sqee/debug/Logging.hpp"  "source/sqee/debug/Logging.cpp"
  "include/sqee/debug/Misc.hpp"     "source/sqee/debug/Misc.cpp"
  "include/sqee/debug/OpenGL.hpp"   "source/sqee/debug/OpenGL.cpp"
  "include/sqee/debug/Text.hpp"     "source/sqee/debug/Text.cpp"

  #----------------------------------------------------------#

  "include/sqee/dop/Classes.hpp"    "source/sqee/dop/Classes.cpp"
  "include/sqee/dop/Functions.hpp"  "source/sqee/dop/Functions.cpp"

  #----------------------------------------------------------#

  "include/sqee/gl/Context.hpp"        "source/sqee/gl/Context.cpp"
  "include/sqee/gl/Drawing.hpp"        "source/sqee/gl/Drawing.cpp"
  "include/sqee/gl/FixedBuffer.hpp"    "source/sqee/gl/FixedBuffer.cpp"
  "include/sqee/gl/FrameBuffer.hpp"    "source/sqee/gl/FrameBuffer.cpp"
  "include/sqee/gl/Program.hpp"        "source/sqee/gl/Program.cpp"
  "include/sqee/gl/TextureBase.hpp"    "source/sqee/gl/TextureBase.cpp"
  "include/sqee/gl/Textures.hpp"       "source/sqee/gl/Textures.cpp"
  "include/sqee/gl/UniformBuffer.hpp"  "source/sqee/gl/UniformBuffer.cpp"
  "include/sqee/gl/VertexArray.hpp"    "source/sqee/gl/VertexArray.cpp"

  #----------------------------------------------------------#

  "include/sqee/maths/Culling.hpp"
  "include/sqee/maths/Functions.hpp"
  "include/sqee/maths/Matrices.hpp"
  "include/sqee/maths/Quaternion.hpp"
  "include/sqee/maths/Scalar.hpp"
  "include/sqee/maths/Vectors.hpp"
  "include/sqee/maths/Volumes.hpp"

  #----------------------------------------------------------#

  "include/sqee/misc/Files.hpp"       "source/sqee/misc/Files.cpp"
  "include/sqee/misc/Json.hpp"        "source/sqee/misc/Json.cpp"
  "include/sqee/misc/StringCast.hpp"  "source/sqee/misc/StringCast.cpp"

  "include/sqee/misc/Algorithms.hpp"
  "include/sqee/misc/PoolTools.hpp"
  "include/sqee/misc/ResourceCache.hpp"
  "include/sqee/misc/ResourceHandle.hpp"

  #----------------------------------------------------------#

  "include/sqee/physics/Bodies.hpp"  "source/sqee/physics/Bodies.cpp"

  #----------------------------------------------------------#

  "include/sqee/render/Armature.hpp"  "source/sqee/render/Armature.cpp"
  "include/sqee/render/Material.hpp"  "source/sqee/render/Material.cpp"
  "include/sqee/render/Mesh.hpp"      "source/sqee/render/Mesh.cpp"
  "include/sqee/render/Volume.hpp"    "source/sqee/render/Volume.cpp"

  #----------------------------------------------------------#

  "include/sqee/scripts/BasicSetup.hpp"  "source/sqee/scripts/BasicSetup.cpp"

  "include/sqee/scripts/Helpers.hpp"

  #----------------------------------------------------------#

  "include/sqee/sound/Sound.hpp"  "source/sqee/sound/Sound.cpp"


  #==========================================================#
  # Compiled-In Data                                         #
  #==========================================================#

  "source/sqee/data/ShaderHeaders.cpp"

  "source/sqee/data/TextFont.c"
  "source/sqee/data/TextGlow.c"

  "source/sqee/data/Volumes.c"
)

################################################################################

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

################################################################################

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/")

include_directories("${PROJECT_SOURCE_DIR}/include")

################################################################################

if (SQEE_STATIC_LIB)
    add_library(sqee STATIC ${SOURCES})
else ()
    add_library(sqee SHARED ${SOURCES})
endif ()

################################################################################

find_package(SFML 2.4 COMPONENTS audio window system REQUIRED)

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(sqee ${SFML_LIBRARIES})

################################################################################

#find_package(ReactPhysics3D REQUIRED)

#include_directories(${RP3D_INCLUDE_DIR})
#target_link_libraries(sqee ${RP3D_LIBRARY})

################################################################################

find_library(OPENGL_LIB NAMES GL opengl32 PATHS /)
find_library(OPENAL_LIB NAMES openal openal32 PATHS /)

target_link_libraries(sqee ${OPENGL_LIB} ${OPENAL_LIB})

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

add_subdirectory("tests")
add_subdirectory("testing")

add_subdirectory("tests/soko")
add_subdirectory("tests/rpg")
