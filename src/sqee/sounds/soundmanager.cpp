#include "sqee/sounds/soundmanager.hpp"

namespace sq {

SoundManager::SoundManager() {
}


void SoundManager::add_sound(string strId, string strPath) {
    soundBufferMap[strId].loadFromFile(strPath);
}

void SoundManager::remove_sound(string) {

}

void SoundManager::play_sound(string strId) {
    soundQueue.push(sf::Sound());
    soundQueue.back().setBuffer(soundBufferMap[strId]);
    soundQueue.back().play();
}

void SoundManager::play_managed_sound(string, string, bool) {

}

void SoundManager::stop_managed_sound(string) {

}

void SoundManager::pause_managed_sound(string) {

}

void SoundManager::resume_managed_sound(string) {

}

void SoundManager::clean() {
    if (!soundQueue.empty()) {
        if (soundQueue.front().getStatus() == sf::Sound::Stopped) {
            soundQueue.pop();
        }
    }
}

}
