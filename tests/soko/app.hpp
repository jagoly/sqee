#ifndef APP_HPP
#define APP_HPP

#include <libsqee/extra.hpp>
#include "level.hpp"

namespace sqt {

class TestApp : public sq::Application {
public:
    TestApp();
    Level* level;

    void load_level(std::string);
};

class HandlerGame : public sq::Handler {
public:
    bool handle(sf::Event&);
};

}

#endif // APP_HPP
