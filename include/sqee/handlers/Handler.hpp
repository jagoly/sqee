#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace sq { class Application; }
namespace sf { class Event; }

namespace sq {

/// The basic SQEE Handler class
class Handler : NonCopyable {
public:
    virtual ~Handler() = default;
    Handler(Application* _app);

    virtual bool handle(sf::Event _event) = 0;
    virtual void refresh() {}

protected:
    Application* const app;
};

}
