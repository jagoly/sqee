#include <iostream>

#include <handler.hpp>

using namespace sq;

Handler::Handler(Application* _app) {
    app = _app;
}

bool Handler::handle(sf::Event&) {
    std::cout << "handler not defined" << std::endl;
    return false;
}
