#include <sqee/app/Application.hpp>
#include <sqee/handlers/Handler.hpp>

using namespace sq;

Handler::Handler(Application* const _app)
    : settings(_app->settings.get()),
      preprocs(_app->preprocs.get()),
      soundman(_app->soundman.get()),
      appBase(_app) {}
