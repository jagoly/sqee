#include <iostream>

#include "eventhandler.hpp"
#include "application.hpp"

using namespace sq;

EventHandler::EventHandler() {

}

bool EventHandler::handle(sf::Event&) {
    std::cout << "handler not defined" << std::endl;
    return false;
}
