set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "app.hpp"                 "app.cpp"
    "scenegame.hpp"           "scenegame.cpp"
    "player.hpp"              "player.cpp"
    "resbank.hpp"             "resbank.cpp"
    "settings.hpp"            "settings.cpp"
    "maps/cell.hpp"           "maps/cell.cpp"
    "maps/world.hpp"          "maps/world.cpp"
    "entities/model.hpp"      "entities/model.cpp"
    "maps/obj/object.hpp"     "maps/obj/object.cpp"
    "maps/obj/model.hpp"      "maps/obj/model.cpp"
    "maps/obj/liquid.hpp"     "maps/obj/liquid.cpp"
    "maps/obj/reflector.hpp"  "maps/obj/reflector.cpp"
    "maps/obj/light.hpp"      "maps/obj/light.cpp"
)

add_executable(rpg ${SOURCES})
target_link_libraries(rpg sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/rpg/")
