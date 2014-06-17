set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/soko")

set(SOURCES
    "main.cpp"
    "helpers.hpp"
    "app.cpp"             "app.hpp"
    "level.cpp"           "level.hpp"
    "scenegame.cpp"       "scenegame.hpp"
    "gamemenus.cpp"       "gamemenus.hpp"
    "mainmenu.cpp"        "mainmenu.hpp"
)

add_executable(soko ${SOURCES})
target_link_libraries(soko libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

add_custom_command(TARGET soko COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/soko/res")
file(GLOB RESOURCES "res/*.*")
foreach(res ${RESOURCES})
    add_custom_command(TARGET soko COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/soko/res")
endforeach()

add_custom_command(TARGET soko COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/soko/res/levels")
file(GLOB RESOURCES "res/levels/*.json")
foreach(res ${RESOURCES})
    add_custom_command(TARGET soko COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/soko/res/levels")
endforeach()
