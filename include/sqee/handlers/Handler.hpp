#pragma once
#include <sqee/forward.hpp>

#include <SFML/Window/Event.hpp>

namespace sq {

class Handler : NonCopyable {
public:
    virtual ~Handler() = default;
    Handler(Application* _app);

    virtual bool handle(const sf::Event& _event) { return false; };
    virtual void update_settings() {};

protected:
    Settings* const settings;
    PreProcessor* const preprocs;
    SoundManager* const soundman;
    Application* const appBase;
};

}
