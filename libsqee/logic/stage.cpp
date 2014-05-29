#include <libsqee/logic/stage.hpp>
#include <libsqee/app/application.hpp>

using namespace sq;

Stage::Stage() {
    tickRate = 60;
    dt = 1.f / 60.f;
}

void Stage::update() {
}
