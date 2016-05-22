#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace sq { class Application; }
namespace sf { class Event; }

namespace sq {

/// The basic SQEE Scene class
class Scene : NonCopyable {
public:
    Scene(Application* _app);
    virtual ~Scene() = default;

    virtual void tick() = 0;
    virtual void render() = 0;
    virtual void configure() {}

    virtual bool handle(sf::Event _event);

    double accum = 0.0;
    uint tickRate = 24u;

protected:
    Application* const app;
};

}
