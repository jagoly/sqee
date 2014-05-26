#include <iostream>

#include <libsqee/tests/soko.hpp>

int main()
{
    sqt::TestApp app;

    sqe::HandlerClose hc;
    sqe::HandlerDebug hd;
    sqt::HandlerGame hg;

    sqt::StageMain stM;
    sqt::StageHud stH;

    sqt::SceneBack scB;
    sqt::SceneFore scF;
    sqt::SceneHud scH;

    scB.stage = &stM;
    scF.stage = &stM;
    scH.stage = &stH;

    app.attach_handler(&hc);
    app.attach_handler(&hd);
    app.attach_handler(&hg);

    app.add_stage("main", &stM);
    app.add_stage("hud", &stH);

    app.append_scene(&scB);
    app.append_scene(&scF);
    app.append_scene(&scH);

    app.set_size({800, 600});

    app.run();

    return 0;
}
