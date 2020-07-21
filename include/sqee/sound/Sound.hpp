#pragma once

//#include <SFML/Audio/Sound.hpp>
//#include <SFML/Audio/SoundBuffer.hpp>

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

#include <sqee/misc/ResourceHandle.hpp>

// did a super quick hack job to stop exposing SFML
// this class is ancient and needs to be rewritten once STS needs sound

namespace sq {

//============================================================================//

struct SQEE_API Listener
{
    static void set_position(Vec3F position);
    static void set_rotation(QuatF rotation);
};

//============================================================================//

/// Wrapper for sf::SoundBuffer.
class SQEE_API SoundWave final : NonCopyable
{
public: //====================================================//

    SoundWave(const String& path);

private: //===================================================//

    //sf::SoundBuffer mBuffer;
    friend class Sound;
};

//============================================================================//

/// Wrapper for sf::Sound.
class SQEE_API Sound final : private MoveOnly
{
public: //====================================================//

    /// Constructor.
    Sound();

    //--------------------------------------------------------//

    /// Set the source SoundWave object.
    void set_wave(Handle<String, SoundWave> handle);

    //--------------------------------------------------------//

    /// Play the sound.
    void play() { /*mSound->play();*/ }

    /// Pause the sound.
    void pause() { /*mSound->pause();*/ }

    /// Stop the sound.
    void stop() { /*mSound->stop();*/ }

    //--------------------------------------------------------//

    /// Check if the sound is playing.
    bool check_playing() const { return false;/*mSound->getStatus() == sf::Sound::Playing;*/ }

    /// Check if the sound is paused.
    bool check_paused() const { return false;/*mSound->getStatus() == sf::Sound::Paused;*/ }

    /// Check if the sound is stopped.
    bool check_stopped() const { return false;/*mSound->getStatus() == sf::Sound::Stopped;*/ }

    //--------------------------------------------------------//

    /// Set the volume of the sound.
    void set_volume(float /*volume*/) { /*mSound->setVolume(volume);*/ }

    /// Set whether the sound's origin is relative to the listener.
    void set_relative(bool /*relative*/) { /*mSound->setRelativeToListener(relative);*/ }

    /// Set whether the sound should loop.
    void set_loop(bool /*loop*/) { /*mSound->setLoop(loop);*/ }

    //--------------------------------------------------------//

    /// Set the origin of the sound.
    void set_position(Vec3F position);

    /// Set fading start and attenuation.
    void set_attenuation(float start, float factor);

private: //===================================================//

    Handle<String, SoundWave> mWaveHandle;
    //UniquePtr<sf::Sound> mSound;
};

//============================================================================//

} // namespace sq
