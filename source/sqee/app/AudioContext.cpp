#include <sqee/app/AudioContext.hpp>

#include <sqee/core/Algorithms.hpp>
#include <sqee/core/Macros.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/objects/Sound.hpp>

#include <sqee/redist/miniaudio.hpp>

#include <mutex>

using namespace sq;

//============================================================================//

struct AudioContext::Implementation
{
    std::mutex mutex;

    ma_device_config config;
    ma_device device;

    static void data_callback(ma_device* pevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
};

//============================================================================//

void AudioContext::Implementation::data_callback(ma_device* pDevice, void* pOutput, const void* /*pInput*/, ma_uint32 frameCount)
{
    auto context = static_cast<AudioContext*>(pDevice->pUserData);
    auto output = static_cast<float*>(pOutput);

    const size_t maxFramesToRead = size_t(frameCount);

    const auto lock = std::lock_guard(context->impl->mutex);

    for (ActiveSound& snd : context->mActiveSounds)
    {
        if (snd.paused || uint8_t(snd.group) & context->mPausedGroups.value)
            continue;

        const size_t framesAvailable = snd.sound->mAudioFrames.size() - snd.cursor;
        const size_t framesToRead = std::min(framesAvailable, maxFramesToRead);

        for (size_t frame = 0u; frame < framesToRead; ++frame, ++snd.cursor)
            output[frame] += snd.sound->mAudioFrames[snd.cursor] * snd.volume;

        if (snd.loop && snd.cursor == snd.sound->mAudioFrames.size())
            snd.cursor = 0u;
    }

    const auto predicate = [](const ActiveSound& snd) { return snd.cursor == snd.sound->mAudioFrames.size(); };
    algo::erase_if(context->mActiveSounds, predicate);
}


//============================================================================//

AudioContext::AudioContext() : impl(std::make_unique<Implementation>())
{
    impl->config = ma_device_config_init(ma_device_type_playback);

    // for now, none of this needs to be configurable
    impl->config.playback.format = ma_format_f32;
    impl->config.playback.channels = 1u;
    impl->config.sampleRate = 48000u;
    impl->config.dataCallback = Implementation::data_callback;
    impl->config.pUserData = this;

    if (auto ec = ma_device_init(NULL, &impl->config, &impl->device); ec != MA_SUCCESS)
        log_error("failed to initialise audio: error code {}", ec);

    log_info("Using MiniAudio Backend '{}'", ma_get_backend_name(impl->device.pContext->backend));

    ma_device_start(&impl->device);
}

//============================================================================//

AudioContext::~AudioContext()
{
    ma_device_uninit(&impl->device);
}

//============================================================================//

int64_t AudioContext::play_sound(const Sound& sound, SoundGroup group, float volume, bool loop)
{
    const auto lock = std::lock_guard(impl->mutex);

    mActiveSounds.push_back({++mCurrentId, &sound, group, volume, loop, false, 0u});
    return mCurrentId;
}

//============================================================================//

void AudioContext::set_sound_paused(int64_t id, bool paused)
{
    SQASSERT(id >= 0 && id <= mCurrentId, "invalid sound id");

    const auto lock = std::lock_guard(impl->mutex);

    const auto predicate = [=](const ActiveSound& snd) { return snd.id == id; };
    if (auto iter = algo::find_if(mActiveSounds, predicate); iter != mActiveSounds.end())
        iter->paused = paused;
}

void AudioContext::stop_sound(int64_t id)
{
    SQASSERT(id >= 0 && id <= mCurrentId, "invalid sound id");

    const auto lock = std::lock_guard(impl->mutex);

    const auto predicate = [=](const ActiveSound& snd) { return snd.id == id; };
    if (auto iter = algo::find_if(mActiveSounds, predicate); iter != mActiveSounds.end())
        mActiveSounds.erase(iter);
}

//============================================================================//

void AudioContext::set_groups_paused(SoundGroups groups, bool pause)
{
    const auto lock = std::lock_guard(impl->mutex);

    if (pause) mPausedGroups = SoundGroups(mPausedGroups.value | groups.value);
    else mPausedGroups = SoundGroups(mPausedGroups.value & ~groups.value);
}

void AudioContext::stop_groups(SoundGroups groups)
{
    const auto lock = std::lock_guard(impl->mutex);

    const auto predicate = [=](const ActiveSound& snd) { return uint8_t(snd.group) & groups.value; };
    algo::erase_if(mActiveSounds, predicate);
}

//============================================================================//

void AudioContext::impl_reset_sound(const Sound* old, const Sound* fresh)
{
    const auto lock = std::lock_guard(impl->mutex);

    if (fresh != nullptr)
    {
        for (ActiveSound& snd : mActiveSounds)
            if (snd.sound == old) snd.sound = fresh;
    }
    else
    {
        // maybe this should be an error instead?
        const auto predicate = [=](const ActiveSound& snd) { return snd.sound == old; };
        algo::erase_if(mActiveSounds, predicate);
    }
}
