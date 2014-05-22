#include <iostream>

#include <SFML/Graphics.hpp>

#include <eventhandler.hpp>
#include <application.hpp>

namespace sqe {

class HandlerClose : public sq::EventHandler {
public:
    bool handle(sf::Event& event) {
        if (event.type == sf::Event::Closed) {
            std::cout << "closing" << std::endl;
            application->running = false;
            return true;
        }
        return false;
    }
};

}
