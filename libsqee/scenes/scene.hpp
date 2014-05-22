#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

namespace sq {

class Scene {
public:
    Scene();

    const sf::Texture& get_tex();

    void set_size(sf::Vector2u);
    void overlay(const sf::Texture&);

    virtual void render();
protected:
    sf::RenderTexture renderTex;
    std::vector<sf::Font> fontVector;
};

}

#endif // SCENE_HPP
