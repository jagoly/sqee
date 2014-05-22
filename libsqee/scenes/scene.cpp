#include <iostream>

#include <libsqee/scenes/scene.hpp>

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

void Scene::overlay(const sf::Texture& tex) {
    static sf::Sprite sprite(tex);
    renderTex.draw(sprite);
}

void Scene::render() {
    renderTex.clear(sf::Color::Transparent);
    static sf::Text text(" Default Scene \n :)", fontVector[0], 40);
    renderTex.draw(text);
    renderTex.display();
}
