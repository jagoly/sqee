#include <libsqee/tests/soko/app.hpp>

using namespace sqt;

TestApp::TestApp() {
    textureHolder.add_texture("playerStill", "test_soko/playerStill.png");
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}
