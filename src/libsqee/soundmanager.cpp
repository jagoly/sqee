#include <soundmanager.hpp>

#include <iostream>

using namespace sq;

SoundManager::SoundManager() {
}


void SoundManager::add_sound(std::string strId, std::string strPath) {
    soundBufferMap[strId].loadFromFile(strPath);
}

void SoundManager::remove_sound(std::string) {

}

void SoundManager::play_sound(std::string strId) {
    soundQueue.push(sf::Sound());
    soundQueue.back().setBuffer(soundBufferMap[strId]);
    soundQueue.back().play();
}

void SoundManager::play_managed_sound(std::string, std::string, bool) {

}

void SoundManager::stop_managed_sound(std::string) {

}

void SoundManager::pause_managed_sound(std::string) {

}

void SoundManager::resume_managed_sound(std::string) {

}

void SoundManager::clean() {
    if (!soundQueue.empty()) {
        if (soundQueue.front().getStatus() == sf::Sound::Stopped) {
            soundQueue.pop();
        }
    }
}
