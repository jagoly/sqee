#pragma once

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq {

/// Draw a point with no attributes
void draw_nothing();

/// Draw a quad to the screen
void draw_screen_quad();

/// Draw a cone volume
void draw_volume_cone();

/// Draw a sphere volume
void draw_volume_sphr();

/// Draw a cube volume
void draw_volume_cube();


// Simple OpenGL function wrappers /////

inline void CLIP_ON() {
    gl::Enable(gl::CLIP_DISTANCE0);
}

inline void CLIP_OFF() {
    gl::Disable(gl::CLIP_DISTANCE0);
}

inline void COLORMASK_ON() {
    gl::ColorMask(false, false, false, false);
}

inline void COLORMASK_OFF() {
    gl::ColorMask(true, true, true, true);
}

inline void BLEND_ON() {
    gl::Enable(gl::BLEND);
}

inline void BLEND_OFF() {
    gl::Disable(gl::BLEND);
}

inline void BLEND_PREM() {
    gl::BlendFunc(gl::ONE, gl::ONE_MINUS_SRC_ALPHA);
}

inline void BLEND_ALPHA() {
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
}

inline void BLEND_ONEONE() {
    gl::BlendFunc(gl::ONE, gl::ONE);
}

inline void DCLAMP_ON() {
    gl::Enable(gl::DEPTH_CLAMP);
}

inline void DCLAMP_OFF() {
    gl::Disable(gl::DEPTH_CLAMP);
}

inline void CULLFACE_ON() {
    gl::Enable(gl::CULL_FACE);
}

inline void CULLFACE_OFF() {
    gl::Disable(gl::CULL_FACE);
}

inline void CULLFACE_BACK() {
    gl::CullFace(gl::BACK);
}

inline void CULLFACE_FRONT() {
    gl::CullFace(gl::FRONT);
}

inline void DEPTH_ON() {
    gl::Enable(gl::DEPTH_TEST);
}

inline void DEPTH_OFF() {
    gl::Disable(gl::DEPTH_TEST);
}

inline void DEPTH_READ() {
    gl::DepthMask(false);
}

inline void DEPTH_WRITE() {
    gl::DepthMask(true);
}

inline void STENCIL_ON() {
    gl::Enable(gl::STENCIL_TEST);
}

inline void STENCIL_OFF() {
    gl::Disable(gl::STENCIL_TEST);
}

inline void STENCIL_KEEP() {
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
}

inline void STENCIL_REPLACE() {
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
}

inline void CLEAR_COLOR() {
    gl::Clear(gl::COLOR_BUFFER_BIT);
}

inline void CLEAR_DEPTH() {
    gl::Clear(gl::DEPTH_BUFFER_BIT);
}

inline void CLEAR_STENC() {
    gl::Clear(gl::STENCIL_BUFFER_BIT);
}

inline void CLEAR_COLOR_DEPTH() {
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
}

inline void CLEAR_COLOR_STENC() {
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);
}

inline void CLEAR_DEPTH_STENC() {
    gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);
}

inline void CLEAR_COLOR_DEPTH_STENC() {
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);
}

inline void VIEWPORT(uint _size) {
    gl::Viewport(0, 0, _size, _size);
}

inline void VIEWPORT(Vec2U _size) {
    gl::Viewport(0, 0, _size.x, _size.y);
}

inline void VIEWPORT(Vec3U _size) {
    gl::Viewport(0, 0, _size.x, _size.y);
}

inline void FRONTFACE(bool _front) {
    gl::FrontFace(_front ? gl::CW : gl::CCW);
}

}
