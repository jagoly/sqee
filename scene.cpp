#include <iostream>

#include <scene.hpp>

using namespace sq;

Scene::Scene() {
    fontVector.push_back(sf::Font());
    fontVector[0].loadFromFile("DroidSans.ttf");
}

const sf::Texture& Scene::get_tex() {
    return renderTex.getTexture();
}

void Scene::set_size(sf::Vector2u size) {
    renderTex.create(size.x, size.y);
}

void Scene::render() {
    renderTex.clear(sf::Color::Black);
    static sf::Text text(" You need to subclass sq::Scene, then\n assign it to your sq::Application.scene", fontVector[0], 40);
    renderTex.draw(text);
    renderTex.display();
}
