#pragma once

#include <sqee/builtins.hpp>
#include <sqee/redist/gl_ext_4_2.hpp>

namespace sq {

/// Draw a point with no attributes
void draw_nothing();

/// Draw a quad to the screen
void draw_screen_quad();


// Simple OpenGL function wrappers /////

inline void FRONTFACE(bool _front) {
    gl::FrontFace(_front ? gl::CW : gl::CCW);
}

inline void DRAW_ELEMENTS_TRIANGLES_U8(uint _count) {
    gl::DrawElements(gl::TRIANGLES, _count, gl::UNSIGNED_BYTE, nullptr);
}

inline void DRAW_ELEMENTS_TRIANGLES_U16(uint _count) {
    gl::DrawElements(gl::TRIANGLES, _count, gl::UNSIGNED_SHORT, nullptr);
}

inline void DRAW_ELEMENTS_TRIANGLES_U32(uint _count) {
    gl::DrawElements(gl::TRIANGLES, _count, gl::UNSIGNED_INT, nullptr);
}

}
