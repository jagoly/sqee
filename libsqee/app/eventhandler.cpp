#include <iostream>

#include <libsqee/app/eventhandler.hpp>
#include <libsqee/app/application.hpp>

using namespace sq;

EventHandler::EventHandler() {

}

bool EventHandler::handle(sf::Event&) {
    std::cout << "handler not defined" << std::endl;
    return false;
}
