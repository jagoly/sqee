#include "menus.hpp"
#include "scenegame.hpp"

namespace sqt {

SceneMenus::SceneMenus(sq::Application* _app) : sq::SceneMenu(_app, false) {
    wHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);

    showScore = true;

    rootWindow->Add(wHBox);
}

void SceneMenus::render(sf::RenderTarget& target, float ft) {
    SceneMenu::render(target, ft);

    static sf::Sprite digitsLargeSprite(app->texHolder.get_texture("digitslarge"));
    digitsLargeSprite.setScale(3, 3);

    if (showScore) {
        int score = static_cast<SceneGame&>(app->get_scene("game")).score;
        int digits = floor(std::log10(std::abs(score))) + 1;
        if (score == 0) digits = 1;

        int scoreX = int(306 - (digits * 39 / 2));
        for (int i = digits; i > 0; i--) {
            digitsLargeSprite.setPosition(scoreX + (digits - i) * 39, 100);
            digitsLargeSprite.setTextureRect({(score / int(std::pow(10, i-1))) % 10 * 12, 0, 12, 18});
            target.draw(digitsLargeSprite);
        }
    }
}

bool HandlerMenus::handle(sf::Event&) {
    return false;
}

}
