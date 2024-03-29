#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/TypeAliases.hpp>

namespace sq { //###############################################################

class AudioContext;

//==============================================================================

/// A sound buffer loaded from a file.
class SQEE_API Sound
{
public: //======================================================

    Sound(AudioContext& context);

    SQEE_COPY_DELETE(Sound)

    Sound(Sound&& other);
    Sound& operator=(Sound&& other);

    ~Sound();

    //----------------------------------------------------------

    /// Load sound data from the specified file.
    void load_from_file(const String& path);

private: //=====================================================

    AudioContext* mContext = nullptr;

    std::vector<float> mAudioFrames;

    friend AudioContext;
};

} // namespace sq ##############################################################
