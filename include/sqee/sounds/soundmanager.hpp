#pragma once
#include "forward.hpp"

#include <map>
#include <queue>

#include <SFML/Audio.hpp>

namespace sq {

class SoundManager {
public:
    SoundManager();

    void add_sound(string strId, string strPath);
    void remove_sound(string strId);

    void play_sound(string strId);
    void play_managed_sound(string strId, string instId, bool notSureWhatThisWasGonnaDo);

    void stop_managed_sound(string instId);
    void pause_managed_sound(string instId);
    void resume_managed_sound(string instId);

    void clean();

private:
    std::map<string, sf::SoundBuffer> soundBufferMap;
    std::map<string, string> soundManagedMap;
    std::queue<sf::Sound> soundQueue;

};

}
