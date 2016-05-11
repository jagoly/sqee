#pragma once

#include <sqee/forward.hpp>
#include <sqee/builtins.hpp>
#include <sqee/scenes/Scene.hpp>
#include <sqee/maths/Vectors.hpp>


namespace sqt {

namespace wcoe { class World; }
namespace rndr { class Renderer; }

class MainScene : public sq::Scene {
public:
    MainScene(sq::Application* _app);

    void update();
    void render();
    void refresh();

    bool handle(sf::Event _event);

private:
    unique_ptr<sq::Pipeline> pipeline;
    unique_ptr<sq::Camera> camera;
    unique_ptr<wcoe::World> world;
    unique_ptr<rndr::Renderer> renderer;

    Vec3F posCrnt, posNext;
    float rotX =0, rotZ =0;

    struct {
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
    } FB;

    struct {
        bool vignette, bloom;
        int shafts, ssao, fsaa;
    } INFO;
};

}
