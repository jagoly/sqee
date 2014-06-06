#pragma once

#include <libsqee/base.hpp>

namespace sqt {

class HandlerMainMenu : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}
