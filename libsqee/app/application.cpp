#include <iostream>
#include <iterator>

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

    window = new sf::RenderWindow(sf::VideoMode(400, 300),
                                  "SQEE DEMO", sf::Style::Default, settings);

    window->setVerticalSyncEnabled(true);
    //window->setFramerateLimit(30);

    sf::Clock FT;

    clock.restart();
    while (true) {
        //std::cout << "\nrender" << std::endl;

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
                stage.second->update();
                stage.second->accum -= 1.f / stage.second->tickRate;
            }
        }

        window->clear(sf::Color::Black);
        static sf::Sprite sprite;
        for (auto& scene : sceneList) {           // TRY OVERLAY METHOD
            scene->render(ft);
            sprite.setTexture(scene->get_tex(), true);
            window->draw(sprite);
        }

        window->display();
    }
}

void Application::set_size(sf::Vector2u size) {
    window->setView(sf::View({0, 0, static_cast<float>(size.x), static_cast<float>(size.y)}));        // CRASHES HERE
    for (auto& scene : sceneList) {
        scene->set_size(size);
    }
}

Stage& Application::get_stage(std::string strId) {
    return *stageMap[strId];
}

//Scene& Application::get_scene(int index) {
//    return *sceneList.begin();
//}

Scene& Application::get_scene() {
    return *sceneList.back();
}

void Application::attach_handler(std::unique_ptr<Handler> handler) {
    handler->application = this;
    handlerFList.push_front(std::move(handler));

}

void Application::append_scene(std::unique_ptr<Scene> scene) {
    scene->textureHolder = &textureHolder;
    sceneList.push_back(std::move(scene));
}

void Application::prepend_scene(std::unique_ptr<Scene> scene) {
    scene->textureHolder = &textureHolder;
    sceneList.push_front(std::move(scene));
}

void Application::insert_scene(int index, std::unique_ptr<Scene> scene) {
    scene->textureHolder = &textureHolder;
    std::list<std::unique_ptr<Scene>>::iterator iter = sceneList.begin();
    std::advance(iter, index);
    sceneList.insert(iter, std::move(scene));
}

void Application::add_stage(std::string strId, std::unique_ptr<Stage> stage) {
    stage->application = this;
    stageMap.insert(std::pair<std::string, std::unique_ptr<Stage>>
                     (strId, std::move(stage)));
}
