#include <SFML/Audio/Listener.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/sound/Sound.hpp>

using namespace sq;

//============================================================================//

void Listener::set_position(Vec3F position)
{
    sf::Listener::setPosition(position.x, position.y, position.z);
}

void Listener::set_rotation(QuatF rotation)
{
    const auto direction = rotation * Vec3F(0.f, 1.f, 0.f);
    const auto tangent = rotation * Vec3F(0.f, 0.f, 1.f);

    sf::Listener::setDirection(direction.x, direction.y, direction.z);
    sf::Listener::setUpVector(tangent.x, tangent.y, tangent.z);
}

//============================================================================//

SoundWave::SoundWave(const string& path)
{
    string fullPath = "assets/sounds/" + path;
    if (check_file_exists(fullPath + ".flac")) fullPath += ".flac";
    else if (check_file_exists(fullPath + ".ogg")) fullPath += ".ogg";
    else log_error("Failed to find sound file '%s'", fullPath);

    mBuffer.loadFromFile(fullPath);
}

//============================================================================//

Sound::Sound()
{
    mSound = std::make_unique<sf::Sound>();
}

void Sound::set_wave(Handle<SoundWave> handle)
{
    SQASSERT(check_stopped(), "sound must be stopped");

    if (handle.check() == false) mSound->resetBuffer();
    else mSound->setBuffer(handle->mBuffer);

    mWaveHandle = handle;
}

void Sound::set_position(Vec3F position)
{
    mSound->setPosition(position.x, position.y, position.z);
}

void Sound::set_attenuation(float start, float factor)
{
    mSound->setMinDistance(start);
    mSound->setAttenuation(factor);
}
