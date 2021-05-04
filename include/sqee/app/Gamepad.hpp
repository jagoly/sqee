// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//============================================================================//

enum class Gamepad_Button : int8_t
{
    Unknown = -1,
    A, B, X, Y, LB, RB,
    Back, Start, Home,
    Up, Right, Down, Left
};

enum class Gamepad_Axis : int8_t
{
    Unknown = -1,
    LX, LY, RX, RY
};

//============================================================================//

struct GamepadState
{
    /// Was the button held during the most recent poll.
    std::array<bool, 13> buttons {};

    /// Maximum absolute value over all polls.
    std::array<float, 4> axes {};
};

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
