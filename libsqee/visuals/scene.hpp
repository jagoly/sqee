#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <string>
#include <memory>

#include <SFML/Graphics.hpp>

#include <libsqee/visuals/textureholder.hpp>
#include <libsqee/logic/stage.hpp>

namespace sq {

class Scene {
public:
    Scene();

    TextureHolder* textureHolder;
    Stage* stage;

    const sf::Texture& get_tex();

    void set_size(sf::Vector2u);

    virtual void render(float);
protected:
    sf::RenderTexture renderTex;
    std::vector<sf::Font> fontVector;
};

}

#endif // SCENE_HPP
