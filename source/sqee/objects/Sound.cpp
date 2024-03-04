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
    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 1u, 48000u);

    std::optional<std::vector<std::byte>> bytes;

    if ((bytes = try_read_bytes_from_file(path + ".wav")))
        decoderConfig.encodingFormat = ma_encoding_format_wav;

    else if ((bytes = try_read_bytes_from_file(path + ".flac")))
        decoderConfig.encodingFormat = ma_encoding_format_flac;

    else if ((bytes = try_read_bytes_from_file(path)))
        decoderConfig.encodingFormat = ma_encoding_format_unknown;

    else SQEE_THROW("could not find sound '{}'", path);

    if (auto ec = ma_decoder_init_memory(bytes->data(), bytes->size(), &decoderConfig, &decoder))
        SQEE_THROW("ma_decoder_init_memory: {}", ma_result_description(ec));

    ma_uint64 numFrames;
    if (auto ec = ma_decoder_get_length_in_pcm_frames(&decoder, &numFrames))
        SQEE_THROW("ma_decoder_get_length_in_pcm_frames: {}", ma_result_description(ec));

    mAudioFrames.resize(size_t(numFrames));

    if (auto ec = ma_decoder_read_pcm_frames(&decoder, mAudioFrames.data(), mAudioFrames.size(), nullptr))
        SQEE_THROW("ma_decoder_read_pcm_frames: {}", ma_result_description(ec));

    ma_decoder_uninit(&decoder);
}
