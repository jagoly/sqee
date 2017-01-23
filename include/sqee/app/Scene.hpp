#pragma once

#include <sqee/setup.hpp>

// Forward Declaration /////
namespace sf { class Event; }

namespace sq {

//============================================================================//

/// The SQEE Scene Class
class Scene : sq::NonCopyable
{
public:

    //========================================================//

    /// Constructor
    Scene(double tickTime);

    /// Virtual Destructor
    virtual ~Scene() = default;

    //========================================================//

    /// called when a global option changes
    virtual void update_options() = 0;

    /// called when an event reaches this scene
    virtual bool handle(sf::Event event) = 0;

    //========================================================//

    /// called once every tick
    virtual void tick() = 0;

    /// called once every frame
    virtual void render() = 0;

    //========================================================//

    double accumulation = 0.0;
    const double tickTime;
};

//============================================================================//

} // namespace sq
