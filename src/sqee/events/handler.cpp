#include <events/handler.hpp>

using namespace sq;

Handler::Handler(Application* _app) : app(_app) {}

bool Handler::handle(sf::Event&) {
    return false;
}
