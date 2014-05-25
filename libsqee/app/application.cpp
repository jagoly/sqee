#include <iostream>

#include <libsqee/app/application.hpp>
#include <libsqee/app/handler.hpp>
#include <libsqee/visuals/scene.hpp>
#include <libsqee/logic/stage.hpp>

using namespace sq;

Application::Application() {
    running = true;
    tickRate = 60;
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
    //window->setFramerateLimit(30);

    sf::Clock DT;
    int queue = 0;
    float add = 0.f;

    clock.restart();
    while (true) {
        float dt = DT.restart().asSeconds();

        window->clear(sf::Color::Black);

        static sf::Sprite sprite;
        std::cout << "render" << std::endl;
        for (auto& scene : sceneVector) {           // TRY OVERLAY METHOD
            scene->render(dt);
            sprite.setTexture(scene->get_tex());
            window->draw(sprite);
        }

        window->display();

        add += tickRate * dt;
        if (add > 1.f) {
            queue++;
            add -= 1.f;
        }

        sf::Event event;
        while (window->pollEvent(event)) {
            for (auto& handler : handlerFList) {
                if (handler->handle(event)) {
                    break;
                }
            }
        }

        if (!running) break;

        while (queue) {
            std::cout << "tick" << std::endl;
            for (auto& stage : stageMap) {
                stage.second->update();
            }
            queue--;
        }
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
