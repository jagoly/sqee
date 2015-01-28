project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(SQEE_USE_BUNDLED_LIBS FALSE CACHE BOOLEAN "Use libraries and headers in the extlibs dir")
set(SQEE_STATIC_LIB FALSE CACHE BOOLEAN "Build SQEE as a static library")
set(SQEE_STATIC_DEPS FALSE CACHE BOOLEAN "Link some libraries statically (varies by platform)")

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter")
if(SQEE_STATIC_DEPS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSFML_STATIC")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    endif()
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSQEE_DEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
endif()

set(SRCDIR "${PROJECT_SOURCE_DIR}/src")
set(INCDIR "${PROJECT_SOURCE_DIR}/include")
set(EMBDIR "${PROJECT_SOURCE_DIR}/embed")

set(SOURCES
# 3rd Party Headers
    "${INCDIR}/sqee/redist/jsoncpp.hpp"
    "${INCDIR}/sqee/redist/stb_image.hpp"

# SQEE Headers
    "${INCDIR}/sqee/defs.hpp"
    "${INCDIR}/sqee/gl/gl_ext_3_3.hpp"
    "${INCDIR}/sqee/gl/gl.hpp"
    "${INCDIR}/sqee/gl/bounds.hpp"
    "${INCDIR}/sqee/gl/cameras.hpp"
    "${INCDIR}/sqee/gl/framebuffers.hpp"
    "${INCDIR}/sqee/gl/shaders.hpp"
    "${INCDIR}/sqee/gl/uniformbuffers.hpp"
    "${INCDIR}/sqee/gl/textures.hpp"
    "${INCDIR}/sqee/app/application.hpp"
    "${INCDIR}/sqee/events/handler.hpp"
    "${INCDIR}/sqee/events/basichandlers.hpp"
    "${INCDIR}/sqee/text/font.hpp"
    "${INCDIR}/sqee/text/text.hpp"
    "${INCDIR}/sqee/maths/glm.hpp"
    "${INCDIR}/sqee/maths/physics.hpp"
    "${INCDIR}/sqee/menus/basicmenu.hpp"
    "${INCDIR}/sqee/models/mesh.hpp"
    "${INCDIR}/sqee/models/skin.hpp"
    "${INCDIR}/sqee/models/skeleton.hpp"
    "${INCDIR}/sqee/misc/files.hpp"
    "${INCDIR}/sqee/misc/containers.hpp"
#    "${INCDIR}/sqee/python/console.hpp"
#    "${INCDIR}/sqee/python/interpreter.hpp"
    "${INCDIR}/sqee/scenes/scene.hpp"
    "${INCDIR}/sqee/scenes/basicscenes.hpp"
    "${INCDIR}/sqee/sounds/soundmanager.hpp"

# 3rd Party Source
    "${SRCDIR}/sqee/redist/jsoncpp.cpp"
    "${SRCDIR}/sqee/redist/stb_image.cpp"

# SQEE Source
    "${SRCDIR}/sqee/gl/gl_ext_3_3.cpp"
    "${SRCDIR}/sqee/gl/gl.cpp"
    "${SRCDIR}/sqee/gl/bounds.cpp"
    "${SRCDIR}/sqee/gl/cameras.cpp"
    "${SRCDIR}/sqee/gl/framebuffers.cpp"
    "${SRCDIR}/sqee/gl/shaders.cpp"
    "${SRCDIR}/sqee/gl/uniformbuffers.cpp"
    "${SRCDIR}/sqee/gl/textures.cpp"
    "${SRCDIR}/sqee/app/application.cpp"
    "${SRCDIR}/sqee/events/basichandlers.cpp"
    "${SRCDIR}/sqee/text/font.cpp"
    "${SRCDIR}/sqee/text/text.cpp"
    "${SRCDIR}/sqee/maths/physics.cpp"
    "${SRCDIR}/sqee/menus/basicmenu.cpp"
    "${SRCDIR}/sqee/models/mesh.cpp"
    "${SRCDIR}/sqee/models/skin.cpp"
    "${SRCDIR}/sqee/models/skeleton.cpp"
    "${SRCDIR}/sqee/misc/files.cpp"
#    "${INCDIR}/sqee/python/console.hpp"
#    "${INCDIR}/sqee/python/interpreter.hpp"
    "${SRCDIR}/sqee/scenes/basicscenes.cpp"
    "${SRCDIR}/sqee/sounds/soundmanager.cpp"
)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/work)
macro (addbin input varname)
    string(REGEX MATCH "([^/]+)$" filename ${EMBDIR}/${input})
    string(REGEX REPLACE "\\.| " "_" filename ${filename})
    file(READ ${EMBDIR}/${input} filedata HEX)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/work/${filename}.c"
               "const unsigned char ${varname}[] = {${filedata}};")
    list(APPEND EMBEDSOURCES "${CMAKE_CURRENT_BINARY_DIR}/work/${filename}.c")
    message(STATUS "Added binary file \"${input}\"")
endmacro ()

addbin("tinytext/texture.bin" ttTexture)
addbin("tinytext/indices.bin" ttIndices)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

include_directories("${PROJECT_SOURCE_DIR}/include/sqee")
if(${SQEE_USE_BUNDLED_LIBS})
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/all")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/lin64")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/win32")
    endif()
endif()

if(${SQEE_STATIC_LIB})
    add_library(sqee STATIC ${SOURCES} ${EMBEDSOURCES})
else()
    add_library(sqee SHARED ${SOURCES} ${EMBEDSOURCES})
endif()

if(${SQEE_USE_BUNDLED_LIBS})
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/extlibs/libs/lin64)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/extlibs/libs/win32)
    endif()
endif()

if(${SQEE_STATIC_DEPS})
    find_library(SFML_WINDOW_LIB sfml-window-s /)
    find_library(SFML_SYSTEM_LIB sfml-system-s /)
    find_library(SFML_AUDIO_LIB sfml-audio-s /)
else()
    find_library(SFML_WINDOW_LIB sfml-window /)
    find_library(SFML_SYSTEM_LIB sfml-system /)
    find_library(SFML_AUDIO_LIB sfml-audio /)
endif()

find_library(OPENGL_LIB NAMES GL opengl32 PATHS /)
find_library(OPENAL_LIB NAMES openal openal32 PATHS /)
find_library(SNDFILE_LIB sndfile /)

target_link_libraries(sqee
    ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${SFML_AUDIO_LIB}
    ${OPENGL_LIB} ${OPENAL_LIB} ${SNDFILE_LIB}
)

if(${SQEE_STATIC_DEPS})
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(sqee pthread udev X11 Xrandr)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        target_link_libraries(sqee winmm gdi32)
    endif()
endif()
	
#add_subdirectory("tests/soko")
#add_subdirectory("tests/fb")
add_subdirectory("tests/gl")
#add_subdirectory("tests/python")
add_subdirectory("tests/rpg")
