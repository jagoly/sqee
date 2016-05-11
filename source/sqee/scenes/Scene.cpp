#include <SFML/Window/Event.hpp>

#include <sqee/app/Application.hpp>
#include <sqee/scenes/Scene.hpp>

using namespace sq;

Scene::Scene(Application* _app) : app(_app) {}

bool Scene::handle(sf::Event _event) { return false; }
