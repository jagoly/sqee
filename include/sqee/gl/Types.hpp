// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <cstddef>
#include <cstdint>

//============================================================================//

typedef char GLchar;
typedef uint8_t GLboolean;
typedef uint32_t GLbitfield;
typedef uint32_t GLenum;

typedef int8_t GLbyte;
typedef int16_t GLshort;
typedef int32_t GLint;
typedef int64_t GLint64;

typedef uint8_t GLubyte;
typedef uint16_t GLushort;
typedef uint32_t GLuint;
typedef uint64_t GLuint64;

typedef float GLfloat;
typedef double GLdouble;

typedef int32_t GLsizei;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

typedef struct __GLsync* GLsync;

//============================================================================//

// easier than messing around with APIENTRY
#ifndef SQEE_MSVC
  #define __stdcall
#endif

typedef void (__stdcall GLDebugCallback)
(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void* userParam);
