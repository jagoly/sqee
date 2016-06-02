#include "Game.hpp"
#include "Title.hpp"
#include "Paused.hpp"
#include "SokoApp.hpp"

using namespace sqt;

SokoApp::SokoApp() {
    activeScenes.emplace_back(new GameScene(*this));
    activeScenes.emplace_back(new TitleScene(*this));
    activeScenes.emplace_back(new PausedScene(*this));
}
