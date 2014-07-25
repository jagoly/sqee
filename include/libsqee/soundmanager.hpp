#pragma once

#include <string>
#include <map>
#include <queue>

#include <SFML/Audio.hpp>

namespace sq {

class SoundManager {
public:
    SoundManager();

    void add_sound(std::string strId, std::string strPath);
    void remove_sound(std::string strId);

    void play_sound(std::string strId);
    void play_managed_sound(std::string strId, std::string instId, bool notSureWhatThisWasGonnaDo);

    void stop_managed_sound(std::string instId);
    void pause_managed_sound(std::string instId);
    void resume_managed_sound(std::string instId);

    void clean();

private:
    std::map<std::string, sf::SoundBuffer> soundBufferMap;
    std::map<std::string, std::string> soundManagedMap;
    std::queue<sf::Sound> soundQueue;

};

}
