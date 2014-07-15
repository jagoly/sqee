#pragma once

#include <string>
#include <map>
#include <queue>

#include <SFML/Audio.hpp>

namespace sq {

class SoundManager {
public:
    SoundManager();

    void add_sound(std::string, std::string);
    void remove_sound(std::string);

    void play_sound(std::string);
    void play_managed_sound(std::string, std::string, bool);

    void stop_managed_sound(std::string);
    void pause_managed_sound(std::string);
    void resume_managed_sound(std::string);

    void clean();

private:
    std::map<std::string, sf::SoundBuffer> soundBufferMap;
    std::map<std::string, std::string> soundManagedMap;
    std::queue<sf::Sound> soundQueue;

};

}
