project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter")

add_definitions(-DSQEE_DEBUG)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")

set(SRCDIR "${PROJECT_SOURCE_DIR}/src")
set(INCDIR "${PROJECT_SOURCE_DIR}/include")
set(BINDIR "${SRCDIR}/sqee/bin")

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
    "${SRCDIR}/sqee/scenes/basicscenes.cpp"
    "${SRCDIR}/sqee/sounds/soundmanager.cpp"
)

MAKE_DIRECTORY(${CMAKE_CURRENT_BINARY_DIR}/objwork)
set(OBJWORKDIR ${CMAKE_CURRENT_BINARY_DIR}/objwork)

add_custom_command(OUTPUT ${OBJWORKDIR}/tinytext_texture.o
    COMMAND cd ${BINDIR} && ld -r -b binary -o ${OBJWORKDIR}/tinytext_texture.o tinytext/texture.bin
)
add_library(tinytext_texture STATIC ${OBJWORKDIR}/tinytext_texture.o)
set_source_files_properties(tinytext_texture PROPERTIES EXTERNAL_OBJECT true GENERATED true)
set_target_properties(tinytext_texture PROPERTIES LINKER_LANGUAGE C)

add_custom_command(OUTPUT ${OBJWORKDIR}/tinytext_indices.o
    COMMAND cd ${BINDIR} && ld -r -b binary -o ${OBJWORKDIR}/tinytext_indices.o tinytext/indices.bin
)
add_library(tinytext_indices STATIC ${OBJWORKDIR}/tinytext_indices.o)
set_source_files_properties(tinytext_indices PROPERTIES EXTERNAL_OBJECT true GENERATED true)
set_target_properties(tinytext_indices PROPERTIES LINKER_LANGUAGE C)


add_library(sqee SHARED ${SOURCES})
include_directories("${PROJECT_SOURCE_DIR}/include/sqee")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/freetype")
    find_library(SFML_SYSTEM_LIB sfml-system "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(SFML_WINDOW_LIB sfml-window "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(SFML_AUDIO_LIB sfml-audio "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(BOOST_SYSTEM_LIB boost_system "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(BOOST_FILESYSTEM_LIB boost_filesystem "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(FREETYPE_LIB freetype "${PROJECT_SOURCE_DIR}/extlibs/lib")
    target_link_libraries(sqee
        ${SFML_SYSTEM_LIB} ${SFML_WINDOW_LIB} ${SFML_AUDIO_LIB}
        ${BOOST_SYSTEM_LIB} ${BOOST_FILESYSTEM_LIB}
        ${FREETYPE_LIB} openGL32
        tinytext_texture tinytext_indices
    )
else ()
    find_package(Freetype REQUIRED)
    include_directories(${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(sqee
        sfml-window sfml-system sfml-audio
        boost_filesystem boost_system
        freetype GL
        tinytext_texture tinytext_indices
    )
endif ()
	
#add_subdirectory("tests/soko")
#add_subdirectory("tests/fb")
add_subdirectory("tests/gl")
add_subdirectory("tests/rpg")
