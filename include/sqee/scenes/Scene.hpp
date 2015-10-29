#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace sq { class Application; }

namespace sq {

/// The basic SQEE Scene class
class Scene : NonCopyable {
public:
    Scene(Application* _app);
    virtual ~Scene() = default;

    virtual void update() = 0;
    virtual void render() = 0;
    virtual void refresh() {}

    double accum = 0.0;
    uint tickRate = 24u;

protected:
    Application* const app;
};

}
