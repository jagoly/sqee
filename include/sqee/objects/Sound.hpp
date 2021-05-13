// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class AudioContext;

//============================================================================//

/// A sound buffer loaded from a file.
class SQEE_API Sound final : private MoveOnly
{
public: //====================================================//

    Sound(AudioContext& context);

    Sound(Sound&& other);
    Sound& operator=(Sound&& other);

    ~Sound();

    //--------------------------------------------------------//

    /// Load sound data from the specified file.
    void load_from_file(const String& path);

private: //===================================================//

    AudioContext* mContext = nullptr;

    std::vector<float> mAudioFrames;

    friend AudioContext;
};

//============================================================================//

} // namespace sq
