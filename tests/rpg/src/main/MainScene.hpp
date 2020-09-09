#pragma once

#include <sqee/app/Scene.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/WrenPlus.hpp>

#include "Options.hpp"
#include "ResourceCaches.hpp"

//====== Forward Declarations ================================================//

namespace sqt { class WorldStuff; class Renderer; class ScriptAPI; }

//============================================================================//

namespace sqt {

class MainScene final : public sq::Scene
{
public: //====================================================//

    MainScene ( const Options& options, wren::WrenPlusVM& vm,
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

    wren::WrenPlusVM& mWrenVM;
    sq::InputDevices& mInput;
    ResourceCaches& mCaches;

    //--------------------------------------------------------//

    std::unique_ptr<WorldStuff> world;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<ScriptAPI> api;

    //--------------------------------------------------------//

    Vec3F mPosCrnt, mPosNext;

    float mRotateX = 0.f;
    float mRotateZ = 0.f;

    bool mEnableInput = true;
};

} // namespace sqt
