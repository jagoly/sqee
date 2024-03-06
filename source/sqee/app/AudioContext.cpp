#include <sqee/app/AudioContext.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/objects/Sound.hpp>

#include <sqee/redist/miniaudio.hpp>

#include <mutex>

using namespace sq;

//==============================================================================

struct AudioContext::Implementation
{
    std::mutex mutex;

    ma_device_config config;
    ma_device device;

    static void data_callback(ma_device* pevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
};

//==============================================================================

void AudioContext::Implementation::data_callback(ma_device* pDevice, void* pOutput, const void* /*pInput*/, ma_uint32 frameCount)
{
    auto context = static_cast<AudioContext*>(pDevice->pUserData);
    auto output = static_cast<float*>(pOutput);

    const uint maxFramesToRead = frameCount;

    const auto lock = std::lock_guard(context->impl->mutex);

    for (ActiveSound& snd : context->mActiveSounds)
    {
        if (snd.paused || context->mPausedGroups.value & uint8_t(snd.group))
            continue;

        const uint framesAvailable = snd.frameCount - snd.cursor;
        const uint framesToRead = std::min(framesAvailable, maxFramesToRead);

        for (uint frame = 0u; frame < framesToRead; ++frame, ++snd.cursor)
            output[frame] += snd.frames[snd.cursor] * snd.volume;

        if (snd.loop && snd.cursor == snd.frameCount)
            snd.cursor = 0u;
    }

    const auto predicate = [](const ActiveSound& snd) { return snd.cursor == snd.frameCount; };
    std::erase_if(context->mActiveSounds, predicate);
}

//==============================================================================

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
        log_error("failed to initialise audio: {}", ma_result_description(ec));

    log_info("Using MiniAudio Backend '{}'", ma_get_backend_name(impl->device.pContext->backend));

    ma_device_start(&impl->device);
}

//==============================================================================

AudioContext::~AudioContext()
{
    ma_device_uninit(&impl->device);
}

//==============================================================================

int32_t AudioContext::play_sound(const Sound& sound, SoundGroup group, float volume, bool loop)
{
    if (mIgnoredGroups.value & uint8_t(group))
        return -1;

    const auto lock = std::lock_guard(impl->mutex);

    ActiveSound& snd = mActiveSounds.emplace_back();
    snd.id = ++mCurrentId;
    snd.cursor = 0u;
    snd.frames = sound.mAudioFrames.data();
    snd.frameCount = uint(sound.mAudioFrames.size());
    snd.group = group;
    snd.volume = volume;
    snd.loop = loop;
    snd.paused = false;

    return mCurrentId;
}

//==============================================================================

void AudioContext::set_sound_paused(int32_t id, bool paused)
{
    SQASSERT(id <= mCurrentId, "invalid id");
    if (id < 0) return;

    const auto lock = std::lock_guard(impl->mutex);

    for (ActiveSound& snd : mActiveSounds)
        if (snd.id == id) { snd.paused = paused; break; }
}

void AudioContext::stop_sound(int32_t id)
{
    SQASSERT(id <= mCurrentId, "invalid id");
    if (id < 0) return;

    const auto lock = std::lock_guard(impl->mutex);

    for (auto iter = mActiveSounds.begin(); iter != mActiveSounds.end(); ++iter)
        if (iter->id == id) { mActiveSounds.erase(iter); break; }
}

//==============================================================================

// todo: could optimise these, since the ranges are all sorted

void AudioContext::set_sounds_paused(std::span<const int32_t> ids, bool paused)
{
    SQASSERT(ranges::all_of(ids, [this](int32_t id) { return id <= mCurrentId; }), "invalid id");
    if (ids.empty()) return;

    const auto lock = std::lock_guard(impl->mutex);

    for (ActiveSound& snd : mActiveSounds)
        if (ranges::find(ids, snd.id) != ids.end())
            snd.paused = paused;
}

void AudioContext::stop_sounds(std::span<const int32_t> ids)
{
    SQASSERT(ranges::all_of(ids, [this](int32_t id) { return id <= mCurrentId; }), "invalid id");
    if (ids.empty()) return;

    const auto lock = std::lock_guard(impl->mutex);

    const auto predicate = [&](const ActiveSound& snd) { return ranges::find(ids, snd.id) != ids.end(); };
    std::erase_if(mActiveSounds, predicate);
}

//==============================================================================

void AudioContext::set_groups_ignored(SoundGroups groups, bool ignore)
{
    if (ignore) mIgnoredGroups = SoundGroups(mIgnoredGroups.value | groups.value);
    else mIgnoredGroups = SoundGroups(mIgnoredGroups.value & ~groups.value);
}

void AudioContext::set_groups_paused(SoundGroups groups, bool pause)
{
    const auto lock = std::lock_guard(impl->mutex);

    if (pause) mPausedGroups = SoundGroups(mPausedGroups.value | groups.value);
    else mPausedGroups = SoundGroups(mPausedGroups.value & ~groups.value);
}

void AudioContext::stop_groups(SoundGroups groups)
{
    const auto lock = std::lock_guard(impl->mutex);

    const auto predicate = [&](const ActiveSound& snd) { return uint8_t(snd.group) & groups.value; };
    std::erase_if(mActiveSounds, predicate);
}

//==============================================================================

void AudioContext::impl_destroy_sound(const Sound& sound)
{
    const auto lock = std::lock_guard(impl->mutex);

    const auto predicate = [&](const ActiveSound& snd) { return snd.frames == sound.mAudioFrames.data(); };
    std::erase_if(mActiveSounds, predicate);
}
