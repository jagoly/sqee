#include <sqee/app/Scene.hpp>

#include <sqee/app/Event.hpp>

using namespace sq;

//============================================================================//

Scene::Scene(double tickTime) : mTickTime(tickTime), mAccumulation(tickTime) {}

//============================================================================//

void Scene::update_and_render(double elapsed)
{
    mAccumulation += elapsed;

    while (mAccumulation >= mTickTime)
    {
        mAccumulation -= mTickTime;
        update();
    }

    render(elapsed);
}

//============================================================================//

void Scene::update_and_integrate(double elapsed)
{
    mAccumulation += elapsed;

    while (mAccumulation >= mTickTime)
    {
        mAccumulation -= mTickTime;
        update();
    }

    integrate(elapsed, float(mAccumulation / mTickTime));
}

//============================================================================//

void Scene::handle_event(Event /*event*/) {}

void Scene::refresh_options() {}

void Scene::show_imgui_widgets() {}
