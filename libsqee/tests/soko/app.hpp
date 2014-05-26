#ifndef APP_HPP
#define APP_HPP

#include <libsqee/extra.hpp>

namespace sqt {

class TestApp : public sq::Application {
public:
    TestApp();
};

class HandlerGame : public sq::Handler {
public:
    bool handle(sf::Event&);
};

}

#endif // APP_HPP
