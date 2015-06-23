#pragma once
#include <sqee/forward.hpp>

namespace sq {

class Scene : NonCopyable {
public:
    virtual ~Scene() = default;
    Scene(Application* _app);

    double accum = 0.0;
    uint tickRate = 24u;

    virtual void update() {};
    virtual void render(float _ft) {};
    virtual void update_settings() {};

protected:
    SettingsMaps* const settings;
    PreProcessor* const preprocs;
    SoundManager* const soundman;
    Application* const appBase;
};

}
