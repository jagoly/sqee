#pragma once

#include <sqee/handlers/Handler.hpp>

namespace sqt {

class MainHandler: public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event event);
};

}
