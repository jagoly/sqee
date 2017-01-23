#include "Game.hpp"
#include "Title.hpp"
#include "Paused.hpp"
#include "SokoApp.hpp"

using namespace sqt;

SokoApp::SokoApp() {
    mScenes.emplace_back(new GameScene(*this));
    mScenes.emplace_back(new TitleScene(*this));
    mScenes.emplace_back(new PausedScene(*this));
}
