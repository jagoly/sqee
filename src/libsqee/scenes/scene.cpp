#include <scenes/scene.hpp>
#include <app/application.hpp>

namespace sq {

Scene::Scene(Application* _app) {
    app = _app;
}

void Scene::resize(glm::uvec2) {
}

void Scene::render(float) {
}

void Scene::update() {
}

float Scene::interpolate(float a, float b) {
    return a * ((dt - accum) / dt) + b * (accum / dt);
}

}
