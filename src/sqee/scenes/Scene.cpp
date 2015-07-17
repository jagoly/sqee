#include <sqee/app/Application.hpp>
#include <sqee/scenes/Scene.hpp>

using namespace sq;

Scene::Scene(Application* _app)
    : settings(_app->settings.get()),
      preprocs(_app->preprocs.get()),
      soundman(_app->soundman.get()),
      appBase(_app) {}
