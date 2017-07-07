#include <sqee/app/Scene.hpp>

using namespace sq;

//============================================================================//

Scene::Scene(double tickTime) : mTickTime(tickTime) {}

//============================================================================//

void Scene::update_and_render(double elapsed)
{
    mAccumulation += elapsed;

    while (mAccumulation >= mTickTime)
    {
        mAccumulation -= mTickTime;
        this->update();
    }

    //--------------------------------------------------------//

    this->render(elapsed);
}
