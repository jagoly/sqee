set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "app.hpp"               "app.cpp"
    "scenegame.hpp"         "scenegame.cpp"
    "rendergeneral.hpp"     "rendergeneral.cpp"
    "player.hpp"            "player.cpp"
    "maps/level.hpp"        "maps/level.cpp"
    "entities/advmodel.hpp" "entities/advmodel.cpp"
    "maps/obj/object.hpp"   "maps/obj/object.cpp"
    "maps/obj/model.hpp"    "maps/obj/model.cpp"
    "maps/obj/liquid.hpp"   "maps/obj/liquid.cpp"
    "maps/obj/data.hpp"     "maps/obj/data.cpp"
    "maps/obj/light.hpp"    "maps/obj/light.cpp"
)

add_executable(rpg ${SOURCES})
target_link_libraries(rpg libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/rpg/")
