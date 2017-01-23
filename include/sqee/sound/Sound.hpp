#pragma once

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/misc/Resource.hpp>

namespace sq {

//============================================================================//

struct Listener
{
    static void set_position(Vec3F position);
    static void set_rotation(QuatF rotation);
};

//============================================================================//

class SoundWave : NonCopyable
{
public:

    SoundWave(const string& path);

private:

    sf::SoundBuffer mBuffer;
    friend class Sound;
};

//============================================================================//

class Sound : public MoveOnly
{
public:

    //========================================================//

    /// constructor
    Sound();

    //========================================================//

    /// set the source sound wave object
    void set_wave(Handle<SoundWave> handle);

    //========================================================//

    /// play the sound
    void play() { mSound->play(); }

    /// pause the sound
    void pause() { mSound->pause(); }

    /// stop the sound
    void stop() { mSound->stop(); }

    //========================================================//

    /// check if the sound is playing
    bool check_playing() const { return mSound->getStatus() == sf::Sound::Playing; }

    /// check if the sound is paused
    bool check_paused() const { return mSound->getStatus() == sf::Sound::Paused; }

    /// check if the sound is stopped
    bool check_stopped() const { return mSound->getStatus() == sf::Sound::Stopped; }

    //========================================================//

    /// set the volume of the sound
    void set_volume(float volume) { mSound->setVolume(volume); }

    /// set cooridante space of the sound
    void set_relative(bool relative) { mSound->setRelativeToListener(relative); }

    /// set looping option of the sound
    void set_loop(bool loop) { mSound->setLoop(loop); }

    //========================================================//

    /// set position of the sound
    void set_position(Vec3F position);

    /// set fade start and attenuation
    void set_attenuation(float start, float factor);

private:

    //========================================================//

    Handle<SoundWave> mWaveHandle;
    unique_ptr<sf::Sound> mSound;
};

//============================================================================//

} // namespace sq
