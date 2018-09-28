// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/app/Scene.hpp>

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
        this->update();
    }

    //--------------------------------------------------------//

    this->render(elapsed);
}
