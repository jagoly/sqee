#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace sf { class Event; }

namespace sq {

class Scene : sq::NonCopyable {
public:
    Scene(double _tickTime);
    virtual ~Scene() = default;

    virtual void update_options() = 0;
    virtual void tick() = 0, render() = 0;
    virtual bool handle(sf::Event _event) = 0;

    double accumulation = 0.0;
    const double tickTime;
};

}
