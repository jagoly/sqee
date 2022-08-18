#include <sqee/objects/Sound.hpp>

#include <sqee/app/AudioContext.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/redist/miniaudio.hpp>

using namespace sq;

//============================================================================//

Sound::Sound(AudioContext& context) : mContext(&context) {}

Sound::Sound(Sound&& other)
{
    *this = std::move(other);
}

Sound& Sound::operator=(Sound&& other)
{
    std::swap(mContext, other.mContext);
    std::swap(mAudioFrames, other.mAudioFrames);
    return *this;
}

Sound::~Sound()
{
    if (mAudioFrames.empty() == false)
        mContext->impl_destroy_sound(*this);
}

//============================================================================//

void Sound::load_from_file(const String& path)
{
    SQASSERT(mAudioFrames.empty(), "sound already loaded");

    ma_decoder decoder;

    // resample all waves to match our output device
    const auto decoderConfig = ma_decoder_config_init(ma_format_f32, 1u, 48000u);

    std::optional<std::vector<std::byte>> bytes;
    ma_result ec;

    if ((bytes = try_read_bytes_from_file(path + ".wav")))
        ec = ma_decoder_init_memory_wav(bytes->data(), bytes->size(), &decoderConfig, &decoder);

    else if ((bytes = try_read_bytes_from_file(path + ".flac")))
        ec = ma_decoder_init_memory_flac(bytes->data(), bytes->size(), &decoderConfig, &decoder);

    else if ((bytes = try_read_bytes_from_file(path)))
        ec = ma_decoder_init_memory(bytes->data(), bytes->size(), &decoderConfig, &decoder);

    else SQEE_THROW("could not find sound '{}'", path);

    if (ec != MA_SUCCESS)
        SQEE_THROW("sound load failure: {}", ma_result_description(ec));

    mAudioFrames.resize(ma_decoder_get_length_in_pcm_frames(&decoder));
    ma_decoder_read_pcm_frames(&decoder, mAudioFrames.data(), mAudioFrames.size());

    ma_decoder_uninit(&decoder);
}
