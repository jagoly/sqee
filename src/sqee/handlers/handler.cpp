#include "sqee/app/application.hpp"
#include "sqee/handlers/handler.hpp"

using namespace sq;

Handler::Handler(Application* const _app)
    : settings(_app->settings.get()),
      preprocs(_app->preprocs.get()),
      soundman(_app->soundman.get()),
      appBase(_app) {}
