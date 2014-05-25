#include <iostream>

#include <libsqee/app/application.hpp>
#include <libsqee/app/handler.hpp>
#include <libsqee/visuals/scene.hpp>
#include <libsqee/logic/stage.hpp>

using namespace sq;

Application::Application() {
    running = true;
}

void Application::run() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    window = new sf::RenderWindow(sf::VideoMode(windowSize.x, windowSize.y),
                                  "SQEE DEMO", sf::Style::Default, settings);
    window->setVerticalSyncEnabled(true);
    //window->setFramerateLimit(60);

    sf::Clock FT;

    clock.restart();
    while (true) {

        std::cout << "\nrender" << std::endl;
        sf::Event event;
        while (window->pollEvent(event)) {
            for (auto& handler : handlerFList) {
                if (handler->handle(event)) {
                    break;
                }
            }
        }
        if (!running) break;

        float ft = FT.restart().asSeconds();

        for (auto& stage : stageMap) {
            stage.second->accum += ft;
            while (stage.second->accum >= 1.f / stage.second->tickRate) {
                std::cout << "calling" << std::endl;

                std::cout << stage.second->accum << std::endl;
                stage.second->update();
                stage.second->accum -= 1.f / stage.second->tickRate;
            }
        }

        window->clear(sf::Color::Black);

        static sf::Sprite sprite;
        for (auto& scene : sceneVector) {           // TRY OVERLAY METHOD
            scene->render(ft);
            sprite.setTexture(scene->get_tex());
            window->draw(sprite);
        }

        window->display();
    }
}

void Application::attach_handler(Handler* handler) {
    handlerFList.push_front(std::unique_ptr<Handler>(std::move(handler)));
    handler->application = this;
}

void Application::append_scene(Scene* scene) {
    sceneVector.push_back(std::unique_ptr<Scene>(std::move(scene)));
    scene->textureHolder = &textureHolder;
}

void Application::add_stage(std::string strId, Stage* stage) {
    stageMap.insert(std::pair<std::string, std::unique_ptr<Stage>>
                     (strId, std::unique_ptr<Stage>(std::move(stage))));
    stage->application = this;
}

void Application::set_size(sf::Vector2u size) {
    windowSize = size;
    for (auto& scene : sceneVector) {
        scene->set_size(size);
    }
}
