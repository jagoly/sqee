#include <iostream>

#include <libsqee/extra.hpp>

using namespace sqe;

bool HandlerClose::handle(sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        application->running = false;
        return true;
    }
    return false;
}

bool HandlerDebug::handle(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::D) {
            for (auto& stage : application->stageMap) {
                for (auto& entity : stage.second->entityMap) {
                    if (entity.second->providesMap.count("debug")) {
                        std::cout << entity.second->actionDebug() << std::endl;
                    }
                }
            }
            return true;
        }
    }
    return false;
}
