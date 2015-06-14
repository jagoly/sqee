#include <sqee/app/application.hpp>
#include <sqee/scenes/scene.hpp>

using namespace sq;

Scene::Scene(Application* const _app)
    : settings(_app->settings.get()),
      preprocs(_app->preprocs.get()),
      soundman(_app->soundman.get()),
      appBase(_app) {}
