#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

#include <sqee/app/Scene.hpp>
#include <sqee/app/ChaiEngine.hpp>
#include <sqee/app/InputDevices.hpp>

#include "Options.hpp"
#include "ResourceCaches.hpp"

//====== Forward Declarations ================================================//

namespace sqt { class WorldStuff; class Renderer; class ScriptAPI; }

//============================================================================//

namespace sqt {

class MainScene final : public sq::Scene
{
public: //====================================================//

    MainScene ( const Options& options, sq::ChaiEngine& engine,
                sq::InputDevices& input, ResourceCaches& caches );

    ~MainScene();

    //--------------------------------------------------------//

    void refresh_options() override;

    void set_input_enabled(bool enable);

private: //===================================================//

    void update() override;

    void render(double elapsed) override;

    //--------------------------------------------------------//

    const Options& options;

    sq::ChaiEngine& mEngine;
    sq::InputDevices& mInput;
    ResourceCaches& mCaches;

    //--------------------------------------------------------//

    UniquePtr<WorldStuff> world;
    UniquePtr<Renderer> renderer;
    UniquePtr<ScriptAPI> api;

    //--------------------------------------------------------//

    Vec3F mPosCrnt, mPosNext;

    float mRotateX = 0.f;
    float mRotateZ = 0.f;

    bool mEnableInput = true;
};

} // namespace sqt
