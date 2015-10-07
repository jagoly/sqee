#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/Scene.hpp>

namespace sqt {

namespace wcoe { class World; }
namespace rndr { class Renderer; }

class MainScene : public sq::Scene {
public:
    MainScene(sq::Application* _app);

    void update();
    void render();
    void refresh();

private:
    unique_ptr<sq::Pipeline> pipeline;
    unique_ptr<sq::Camera> camera;
    unique_ptr<wcoe::World> world;
    unique_ptr<rndr::Renderer> renderer;

    fvec3 posCrnt, posNext;
    float rotX =0, rotZ =0;


    struct {
        unique_ptr<sq::TextureMut2D> pshadA;
        unique_ptr<sq::TextureMut2D> pshadB;
        unique_ptr<sq::TextureMut2D> reflDiff;
        unique_ptr<sq::TextureMut2D> reflSurf;
        unique_ptr<sq::TextureMut2D> reflDpSt;
        unique_ptr<sq::TextureMut2D> partMain;
        unique_ptr<sq::TextureMut2D> partDpSt;
        unique_ptr<sq::TextureMut2D> hdrRefl;
        unique_ptr<sq::TextureMut2D> hdrPart;
    } TX;

    struct {
        unique_ptr<sq::FrameBuffer> pshadA;
        unique_ptr<sq::FrameBuffer> pshadB;
        unique_ptr<sq::FrameBuffer> defrRefl;
        unique_ptr<sq::FrameBuffer> defrPart;
        unique_ptr<sq::FrameBuffer> hdrRefl;
        unique_ptr<sq::FrameBuffer> hdrPart;
    } FB;

    struct {
        float viewdist;
        bool shadfilter;
        bool shadlarge;
        bool vignette;
        bool bloom;
        int shafts;
        int ssao;
        int fsaa;
        uvec2 fullSize;
        uvec2 halfSize;
        uvec2 qterSize;
        fvec2 fPixSize;
        fvec2 hPixSize;
        fvec2 qPixSize;
        float aspect;
    } INFO;

    void reload_shaders();
};

}
