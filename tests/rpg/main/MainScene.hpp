#pragma once

#include <sqee/builtins.hpp>
#include <sqee/messages.hpp>

#include <sqee/maths/Vectors.hpp>

#include <sqee/app/Scene.hpp>
#include <sqee/app/MessageBus.hpp>

namespace sqt {

//============================================================================//

// Forward Declarations /////
class RpgApp; class World; class Renderer;

//============================================================================//

class MainScene : public sq::Scene
{
public:

    //========================================================//

    MainScene(RpgApp& app);

    void update_options();
    bool handle(sf::Event event);
    void tick(); void render();

private:

    //========================================================//

    RpgApp& mRpgApp;

    unique_ptr<World> world;
    unique_ptr<Renderer> renderer;

    Vec3F mPosCrnt, mPosNext;

    float mRotateX = 0.f;
    float mRotateZ = 0.f;

    sq::Receiver<msg::Toggle_Chai_Console> on_Toggle_Chai_Console;
};

//============================================================================//

} // namespace sqt
