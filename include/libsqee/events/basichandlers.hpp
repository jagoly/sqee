#pragma once

#include <events/handler.hpp>
#include <app/application.hpp>

namespace sq {

class HandlerClose : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

class HandlerDebug : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

class HandlerResize : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

class HandlerFramelimit : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

}
