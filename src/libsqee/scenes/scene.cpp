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

template<typename T>
T Scene::interpolate(T a, T b) {
    return a * ((dt - accum) / dt) + b * (accum / dt);
}

}
