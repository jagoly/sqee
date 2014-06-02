project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")

set(SRCDIR "${PROJECT_SOURCE_DIR}/src")
set(INCDIR "${PROJECT_SOURCE_DIR}/include")

set(SFML_LIBS sfml-window sfml-graphics sfml-system)

set(SOURCES
    "${SRCDIR}/libsqee/app/application.cpp"
    "${SRCDIR}/libsqee/app/handler.cpp"
    "${SRCDIR}/libsqee/basics/handlers.cpp"
    "${SRCDIR}/libsqee/logic/entity.cpp"
    "${SRCDIR}/libsqee/logic/stage.cpp"
    "${SRCDIR}/libsqee/visuals/scene.cpp"
    "${SRCDIR}/libsqee/visuals/textureholder.cpp"
    "${INCDIR}/libsqee/app/application.hpp"
    "${INCDIR}/libsqee/app/handler.hpp"
    "${INCDIR}/libsqee/basics/handlers.hpp"
    "${INCDIR}/libsqee/logic/entity.hpp"
    "${INCDIR}/libsqee/logic/stage.hpp"
    "${INCDIR}/libsqee/visuals/scene.hpp"
    "${INCDIR}/libsqee/visuals/textureholder.hpp"
)

add_library(libsqee SHARED ${SOURCES})
link_libraries(${SFML_LIBS})
include_directories("${PROJECT_SOURCE_DIR}/include/libsqee")
set_target_properties(libsqee PROPERTIES PREFIX "")

add_subdirectory("tests/soko")
