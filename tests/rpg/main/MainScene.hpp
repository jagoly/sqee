#pragma once

#include <sqee/builtins.hpp>
#include <sqee/scenes/Scene.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/app/MessageBus.hpp>

namespace sqt {

// Forward Declarations /////
class RpgApp; class World; class Renderer;

class MainScene : public sq::Scene {
public:
    MainScene(RpgApp& _app);

    void update_options();
    void tick(); void render();
    bool handle(sf::Event _event);

private:
    Vec3F posCrnt, posNext;
    float rotX =0, rotZ =0;

    RpgApp& app;

    unique_ptr<sq::MessageBus> mbus;

    unique_ptr<World> world;
    unique_ptr<Renderer> renderer;

    /*struct {
        unique_ptr<sq::Texture2D> pshadA;
        unique_ptr<sq::Texture2D> pshadB;
        unique_ptr<sq::Texture2D> partMain;
        unique_ptr<sq::Texture2D> partDpSt;
        unique_ptr<sq::Texture2D> hdrPart;
    } TX;

    struct {
        unique_ptr<sq::FrameBuffer> pshadA;
        unique_ptr<sq::FrameBuffer> pshadB;
        unique_ptr<sq::FrameBuffer> defrPart;
        unique_ptr<sq::FrameBuffer> hdrPart;
    } FB;*/
};

}
