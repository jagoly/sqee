#pragma once

#include <memory>

#include <SFML/Window.hpp>

#define SQ_KEYPRESS(keyName)   (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::keyName)
#define SQ_KEYRELEASE(keyName) (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::keyName)

#define SQ_LEFTMPRESS    (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
#define SQ_LEFTMRELEASE  (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
#define SQ_RIGHTMPRESS   (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
#define SQ_RIGHTMRELEASE (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)

namespace sq {

class Application;

class Handler {
public:
    typedef std::unique_ptr<Handler> Ptr;

    Handler(Application* _app);

    virtual bool handle(sf::Event& event);

protected:
    Application* app;
};

}
