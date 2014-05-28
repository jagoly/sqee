#include <iostream>

#include <libsqee/app/handler.hpp>
#include <libsqee/app/application.hpp>

using namespace sq;

Handler::Handler() {
}

bool Handler::handle(sf::Event&) {
    std::cout << "handler not defined" << std::endl;
    return false;
}
