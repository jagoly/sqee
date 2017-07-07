#pragma once

#include <sqee/builtins.hpp>

#include <sqee/maths/Vectors.hpp>

#include <sqee/app/Scene.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/app/ChaiEngine.hpp>
#include <sqee/app/InputDevices.hpp>


namespace sqt {

//============================================================================//

// Forward Declarations /////
class Options; class World; class Renderer; class ResourceCaches; class WorldStuff; class ScriptAPI;

//============================================================================//

class MainScene final : public sq::Scene
{
public: //====================================================//

    MainScene ( sq::MessageBus& bus, sq::ChaiEngine& engine,
                sq::InputDevices& input, ResourceCaches& caches,
                const Options& options );

    ~MainScene();

    //--------------------------------------------------------//

    void refresh_options();

    void set_input_enabled(bool enable);

private: //===================================================//

    void update() override;

    void render(double elapsed) override;

    //--------------------------------------------------------//

    sq::MessageBus& mBus;
    sq::ChaiEngine& mEngine;
    sq::InputDevices& mInput;
    ResourceCaches& mCaches;

    const Options& options;

    //--------------------------------------------------------//

    unique_ptr<WorldStuff> world;
    unique_ptr<Renderer> renderer;
    unique_ptr<ScriptAPI> api;

    //--------------------------------------------------------//

    Vec3F mPosCrnt, mPosNext;

    float mRotateX = 0.f;
    float mRotateZ = 0.f;

    bool mEnableInput = true;
};

//============================================================================//

} // namespace sqt
