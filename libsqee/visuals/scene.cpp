#include <iostream>

#include <libsqee/visuals/scene.hpp>
#include <libsqee/logic/stage.hpp>

using namespace sq;

Scene::Scene() {
}

const sf::Texture& Scene::get_tex() {
    return renderTex.getTexture();
}

void Scene::set_size(sf::Vector2u size) {
    renderTex.create(size.x, size.y);
}

void Scene::render(float) {
    renderTex.clear(sf::Color::Yellow);
    renderTex.display();
}
