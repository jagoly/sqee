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

    Sound(AudioContext& context); ///< Constructor.

    Sound(Sound&& other) noexcept; ///< Move Constructor.

    Sound& operator=(Sound&& other) noexcept; ///< Move Assignment.

    ~Sound() noexcept; ///< Destructor.

    /// Load sound data from the specified file.
    void load_from_file(const String& path);

private: //===================================================//

    AudioContext& mContext;

    std::vector<float> mAudioFrames;

    friend AudioContext;
};

//============================================================================//

} // namespace sq
