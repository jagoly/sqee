// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/EnumHelper.hpp>

namespace sq {

//============================================================================//

enum class Gamepad_Button : int8_t
{
    Unknown = -1,
    A, B, X, Y,
    LeftBump, RightBump,
    Back, Start, Guide,
    LeftStick, RightStick,
    Up, Right, Down, Left
};

enum class Gamepad_Axis : int8_t
{
    Unknown = -1,
    LeftX, LeftY,
    RightX, RightY,
    LeftTrigger,
    RightTrigger
};

//============================================================================//

/// The raw state of a gamepad.
struct GamepadState
{
    /// Was the button held during the most recent poll.
    std::array<bool, 15> buttons {};

    /// Maximum absolute value over all polls.
    std::array<float, 6> axes {};
};

/// The state of a gamepad, including press and release data.
struct Gamepad : public GamepadState
{
    /// Did button go from not held to held since the last clear.
    std::array<bool, 13> pressed {};

    /// Did button go from held to not held since the last clear.
    std::array<bool, 13> released {};

    /// Integrate state from a poll.
    void integrate(const GamepadState& state)
    {
        for (size_t i = 0u; i < 13u; ++i)
        {
            pressed[i] |= !buttons[i] && state.buttons[i];
            released[i] |= buttons[i] && !state.buttons[i];
            buttons[i] = state.buttons[i];
        }

        for (size_t i = 0u; i < 4u; ++i)
        {
            // greater or equal, so in case of a tie use most recent
            if (std::abs(state.axes[i]) >= std::abs(axes[i]))
                axes[i] = state.axes[i];
        }
    }

    /// Clear everything except held buttons.
    void finish_tick()
    {
        pressed.fill(false);
        released.fill(false);
        axes.fill(0.f);
    }
};

//============================================================================//

} // namespace sq

SQEE_ENUM_HELPER
(
    sq::Gamepad_Button,
    Unknown,
    A, B, X, Y,
    LeftBump, RightBump,
    Back, Start, Guide,
    LeftStick, RightStick,
    Up, Right, Down, Left
)


SQEE_ENUM_HELPER
(
    sq::Gamepad_Axis,
    Unknown,
    LeftX, LeftY,
    RightX, RightY,
    LeftTrigger,
    RightTrigger
)
