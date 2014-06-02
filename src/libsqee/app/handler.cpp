#include <iostream>

#include <app/handler.hpp>
#include <app/application.hpp>

using namespace sq;

Handler::Handler() {
}

bool Handler::handle(sf::Event&) {
    std::cout << "handler not defined" << std::endl;
    return false;
}
