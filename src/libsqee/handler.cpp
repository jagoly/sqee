#include <handler.hpp>
#include <application.hpp>

namespace sq {

Handler::Handler(Application* _app) {
    app = _app;
}

bool Handler::handle(sf::Event&) {
    return false;
}

}
