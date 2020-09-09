#include <sqee/objects/Sound.hpp>

#include <sqee/app/AudioContext.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/redist/miniaudio.hpp>

using namespace sq;

//============================================================================//

static constexpr const char* conv_ma_error_code[]
{
    "MA_SUCCESS",
    "MA_ERROR",
    "MA_INVALID_ARGS",
    "MA_INVALID_OPERATION",
    "MA_OUT_OF_MEMORY",
    "MA_OUT_OF_RANGE",
    "MA_ACCESS_DENIED",
    "MA_DOES_NOT_EXIST",
    "MA_ALREADY_EXISTS",
    "MA_TOO_MANY_OPEN_FILES",
    "MA_INVALID_FILE",
    "MA_TOO_BIG",
    "MA_PATH_TOO_LONG",
    "MA_NAME_TOO_LONG",
    "MA_NOT_DIRECTORY",
    "MA_IS_DIRECTORY",
    "MA_DIRECTORY_NOT_EMPTY",
    "MA_END_OF_FILE",
    "MA_NO_SPACE",
    "MA_BUSY",
    "MA_IO_ERROR",
    "MA_INTERRUPT",
    "MA_UNAVAILABLE",
    "MA_ALREADY_IN_USE",
    "MA_BAD_ADDRESS",
    "MA_BAD_SEEK",
    "MA_BAD_PIPE",
    "MA_DEADLOCK",
    "MA_TOO_MANY_LINKS",
    "MA_NOT_IMPLEMENTED",
    "MA_NO_MESSAGE",
    "MA_BAD_MESSAGE",
    "MA_NO_DATA_AVAILABLE",
    "MA_INVALID_DATA",
    "MA_TIMEOUT",
    "MA_NO_NETWORK",
    "MA_NOT_UNIQUE",
    "MA_NOT_SOCKET",
    "MA_NO_ADDRESS",
    "MA_BAD_PROTOCOL",
    "MA_PROTOCOL_UNAVAILABLE",
    "MA_PROTOCOL_NOT_SUPPORTED",
    "MA_PROTOCOL_FAMILY_NOT_SUPPORTED",
    "MA_ADDRESS_FAMILY_NOT_SUPPORTED",
    "MA_SOCKET_NOT_SUPPORTED",
    "MA_CONNECTION_RESET",
    "MA_ALREADY_CONNECTED",
    "MA_NOT_CONNECTED",
    "MA_CONNECTION_REFUSED",
    "MA_NO_HOST",
    "MA_IN_PROGRESS",
    "MA_CANCELLED",
    "MA_MEMORY_ALREADY_MAPPED",
    "MA_AT_END"
};

//============================================================================//

Sound::Sound(AudioContext& context)
    : mContext(context) {}

Sound::Sound(Sound&& other) noexcept :
    mContext(other.mContext)
{
    mAudioFrames = std::move(other.mAudioFrames);
    mContext.impl_reset_sound(&other, this);
};

Sound& Sound::operator=(Sound&& other) noexcept
{
    std::swap(*this, other);
    return *this;
}

Sound::~Sound() noexcept
{
    mContext.impl_reset_sound(this);
}

//============================================================================//

void Sound::load_from_file(const String& path)
{
    mContext.impl_reset_sound(this);

    // resample all waves to match our output device
    const auto decoderConfig = ma_decoder_config_init(ma_format_f32, 1u, 48000u);

    ma_decoder decoder;

    if (auto ec = ma_decoder_init_file(path.c_str(), &decoderConfig, &decoder); ec != MA_SUCCESS)
    {
        ma_decoder_uninit(&decoder); // not sure if we need to free here, but it can't hurt
        throw std::runtime_error(fmt::format("failed to load sound '{}': {}", path, conv_ma_error_code[-ec]));
    }

    mAudioFrames.resize(ma_decoder_get_length_in_pcm_frames(&decoder));
    ma_decoder_read_pcm_frames(&decoder, mAudioFrames.data(), mAudioFrames.size());

    ma_decoder_uninit(&decoder);
}
