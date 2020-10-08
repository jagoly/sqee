#pragma once

#include <sqee/export.hpp>

#include <sqee/gl/Types.hpp> // IWYU pragma: export

namespace sq::gl::detail {

//============================================================================//

// OpenGL 1.0
SQEE_API extern void (__stdcall* glCullFace)(GLenum mode);
SQEE_API extern void (__stdcall* glFrontFace)(GLenum mode);
SQEE_API extern void (__stdcall* glHint)(GLenum target, GLenum mode);
SQEE_API extern void (__stdcall* glLineWidth)(GLfloat width);
SQEE_API extern void (__stdcall* glPointSize)(GLfloat size);
SQEE_API extern void (__stdcall* glPolygonMode)(GLenum face, GLenum mode);
SQEE_API extern void (__stdcall* glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
SQEE_API extern void (__stdcall* glTexParameterfv)(GLenum target, GLenum pname, const GLfloat* params);
SQEE_API extern void (__stdcall* glTexParameteri)(GLenum target, GLenum pname, GLint param);
SQEE_API extern void (__stdcall* glTexParameteriv)(GLenum target, GLenum pname, const GLint* params);
SQEE_API extern void (__stdcall* glTexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glDrawBuffer)(GLenum buf);
SQEE_API extern void (__stdcall* glClear)(GLbitfield mask);
SQEE_API extern void (__stdcall* glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
SQEE_API extern void (__stdcall* glClearStencil)(GLint s);
SQEE_API extern void (__stdcall* glClearDepth)(GLdouble depth);
SQEE_API extern void (__stdcall* glStencilMask)(GLuint mask);
SQEE_API extern void (__stdcall* glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
SQEE_API extern void (__stdcall* glDepthMask)(GLboolean flag);
SQEE_API extern void (__stdcall* glDisable)(GLenum cap);
SQEE_API extern void (__stdcall* glEnable)(GLenum cap);
SQEE_API extern void (__stdcall* glFinish)();
SQEE_API extern void (__stdcall* glFlush)();
SQEE_API extern void (__stdcall* glBlendFunc)(GLenum sfactor, GLenum dfactor);
SQEE_API extern void (__stdcall* glLogicOp)(GLenum opcode);
SQEE_API extern void (__stdcall* glStencilFunc)(GLenum func, GLint ref, GLuint mask);
SQEE_API extern void (__stdcall* glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
SQEE_API extern void (__stdcall* glDepthFunc)(GLenum func);
SQEE_API extern void (__stdcall* glPixelStoref)(GLenum pname, GLfloat param);
SQEE_API extern void (__stdcall* glPixelStorei)(GLenum pname, GLint param);
SQEE_API extern void (__stdcall* glReadBuffer)(GLenum src);
SQEE_API extern void (__stdcall* glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);
SQEE_API extern void (__stdcall* glGetBooleanv)(GLenum pname, GLboolean* data);
SQEE_API extern void (__stdcall* glGetDoublev)(GLenum pname, GLdouble* data);
SQEE_API extern GLenum (__stdcall* glGetError)();
SQEE_API extern void (__stdcall* glGetFloatv)(GLenum pname, GLfloat* data);
SQEE_API extern void (__stdcall* glGetIntegerv)(GLenum pname, GLint* data);
SQEE_API extern const GLubyte* (__stdcall* glGetString)(GLenum name);
SQEE_API extern void (__stdcall* glGetTexImage)(GLenum target, GLint level, GLenum format, GLenum type, void* pixels);
SQEE_API extern void (__stdcall* glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat* params);
SQEE_API extern void (__stdcall* glGetTexParameteriv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat* params);
SQEE_API extern void (__stdcall* glGetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint* params);
SQEE_API extern GLboolean (__stdcall* glIsEnabled)(GLenum cap);
SQEE_API extern void (__stdcall* glDepthRange)(GLdouble n, GLdouble f);
SQEE_API extern void (__stdcall* glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

// OpenGL 1.1
SQEE_API extern void (__stdcall* glDrawArrays)(GLenum mode, GLint first, GLsizei count);
SQEE_API extern void (__stdcall* glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void* indices);
SQEE_API extern void (__stdcall* glPolygonOffset)(GLfloat factor, GLfloat units);
SQEE_API extern void (__stdcall* glCopyTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
SQEE_API extern void (__stdcall* glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
SQEE_API extern void (__stdcall* glCopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
SQEE_API extern void (__stdcall* glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glBindTexture)(GLenum target, GLuint texture);
SQEE_API extern void (__stdcall* glDeleteTextures)(GLsizei n, const GLuint* textures);
SQEE_API extern void (__stdcall* glGenTextures)(GLsizei n, GLuint* textures);
SQEE_API extern GLboolean (__stdcall* glIsTexture)(GLuint texture);

// OpenGL 1.2
SQEE_API extern void (__stdcall* glDrawRangeElements)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
SQEE_API extern void (__stdcall* glTexImage3D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glCopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

// OpenGL 1.3
SQEE_API extern void (__stdcall* glActiveTexture)(GLenum texture);
SQEE_API extern void (__stdcall* glSampleCoverage)(GLfloat value, GLboolean invert);
SQEE_API extern void (__stdcall* glCompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glCompressedTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glCompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glCompressedTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glGetCompressedTexImage)(GLenum target, GLint level, void* img);

// OpenGL 1.4
SQEE_API extern void (__stdcall* glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
SQEE_API extern void (__stdcall* glMultiDrawArrays)(GLenum mode, const GLint* first, const GLsizei* count, GLsizei drawcount);
SQEE_API extern void (__stdcall* glMultiDrawElements)(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount);
SQEE_API extern void (__stdcall* glPointParameterf)(GLenum pname, GLfloat param);
SQEE_API extern void (__stdcall* glPointParameterfv)(GLenum pname, const GLfloat* params);
SQEE_API extern void (__stdcall* glPointParameteri)(GLenum pname, GLint param);
SQEE_API extern void (__stdcall* glPointParameteriv)(GLenum pname, const GLint* params);
SQEE_API extern void (__stdcall* glBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
SQEE_API extern void (__stdcall* glBlendEquation)(GLenum mode);

// OpenGL 1.5
SQEE_API extern void (__stdcall* glGenQueries)(GLsizei n, GLuint* ids);
SQEE_API extern void (__stdcall* glDeleteQueries)(GLsizei n, const GLuint* ids);
SQEE_API extern GLboolean (__stdcall* glIsQuery)(GLuint id);
SQEE_API extern void (__stdcall* glBeginQuery)(GLenum target, GLuint id);
SQEE_API extern void (__stdcall* glEndQuery)(GLenum target);
SQEE_API extern void (__stdcall* glGetQueryiv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetQueryObjectiv)(GLuint id, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint* params);
SQEE_API extern void (__stdcall* glBindBuffer)(GLenum target, GLuint buffer);
SQEE_API extern void (__stdcall* glDeleteBuffers)(GLsizei n, const GLuint* buffers);
SQEE_API extern void (__stdcall* glGenBuffers)(GLsizei n, GLuint* buffers);
SQEE_API extern GLboolean (__stdcall* glIsBuffer)(GLuint buffer);
SQEE_API extern void (__stdcall* glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
SQEE_API extern void (__stdcall* glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
SQEE_API extern void (__stdcall* glGetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, void* data);
SQEE_API extern void* (__stdcall* glMapBuffer)(GLenum target, GLenum access);
SQEE_API extern GLboolean (__stdcall* glUnmapBuffer)(GLenum target);
SQEE_API extern void (__stdcall* glGetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetBufferPointerv)(GLenum target, GLenum pname, void** params);

// OpenGL 2.0
SQEE_API extern void (__stdcall* glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
SQEE_API extern void (__stdcall* glDrawBuffers)(GLsizei n, const GLenum* bufs);
SQEE_API extern void (__stdcall* glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
SQEE_API extern void (__stdcall* glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
SQEE_API extern void (__stdcall* glStencilMaskSeparate)(GLenum face, GLuint mask);
SQEE_API extern void (__stdcall* glAttachShader)(GLuint program, GLuint shader);
SQEE_API extern void (__stdcall* glBindAttribLocation)(GLuint program, GLuint index, const GLchar* name);
SQEE_API extern void (__stdcall* glCompileShader)(GLuint shader);
SQEE_API extern GLuint (__stdcall* glCreateProgram)();
SQEE_API extern GLuint (__stdcall* glCreateShader)(GLenum type);
SQEE_API extern void (__stdcall* glDeleteProgram)(GLuint program);
SQEE_API extern void (__stdcall* glDeleteShader)(GLuint shader);
SQEE_API extern void (__stdcall* glDetachShader)(GLuint program, GLuint shader);
SQEE_API extern void (__stdcall* glDisableVertexAttribArray)(GLuint index);
SQEE_API extern void (__stdcall* glEnableVertexAttribArray)(GLuint index);
SQEE_API extern void (__stdcall* glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
SQEE_API extern void (__stdcall* glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
SQEE_API extern void (__stdcall* glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
SQEE_API extern GLint (__stdcall* glGetAttribLocation)(GLuint program, const GLchar* name);
SQEE_API extern void (__stdcall* glGetProgramiv)(GLuint program, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
SQEE_API extern void (__stdcall* glGetShaderiv)(GLuint shader, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
SQEE_API extern void (__stdcall* glGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
SQEE_API extern GLint (__stdcall* glGetUniformLocation)(GLuint program, const GLchar* name);
SQEE_API extern void (__stdcall* glGetUniformfv)(GLuint program, GLint location, GLfloat* params);
SQEE_API extern void (__stdcall* glGetUniformiv)(GLuint program, GLint location, GLint* params);
SQEE_API extern void (__stdcall* glGetVertexAttribdv)(GLuint index, GLenum pname, GLdouble* params);
SQEE_API extern void (__stdcall* glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);
SQEE_API extern void (__stdcall* glGetVertexAttribiv)(GLuint index, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetVertexAttribPointerv)(GLuint index, GLenum pname, void** pointer);
SQEE_API extern GLboolean (__stdcall* glIsProgram)(GLuint program);
SQEE_API extern GLboolean (__stdcall* glIsShader)(GLuint shader);
SQEE_API extern void (__stdcall* glLinkProgram)(GLuint program);
SQEE_API extern void (__stdcall* glShaderSource)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
SQEE_API extern void (__stdcall* glUseProgram)(GLuint program);
SQEE_API extern void (__stdcall* glUniform1f)(GLint location, GLfloat v0);
SQEE_API extern void (__stdcall* glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
SQEE_API extern void (__stdcall* glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
SQEE_API extern void (__stdcall* glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
SQEE_API extern void (__stdcall* glUniform1i)(GLint location, GLint v0);
SQEE_API extern void (__stdcall* glUniform2i)(GLint location, GLint v0, GLint v1);
SQEE_API extern void (__stdcall* glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
SQEE_API extern void (__stdcall* glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
SQEE_API extern void (__stdcall* glUniform1fv)(GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniform2fv)(GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniform3fv)(GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniform4fv)(GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniform1iv)(GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (__stdcall* glUniform2iv)(GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (__stdcall* glUniform3iv)(GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (__stdcall* glUniform4iv)(GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (__stdcall* glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glValidateProgram)(GLuint program);
SQEE_API extern void (__stdcall* glVertexAttrib1d)(GLuint index, GLdouble x);
SQEE_API extern void (__stdcall* glVertexAttrib1dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (__stdcall* glVertexAttrib1f)(GLuint index, GLfloat x);
SQEE_API extern void (__stdcall* glVertexAttrib1fv)(GLuint index, const GLfloat* v);
SQEE_API extern void (__stdcall* glVertexAttrib1s)(GLuint index, GLshort x);
SQEE_API extern void (__stdcall* glVertexAttrib1sv)(GLuint index, const GLshort* v);
SQEE_API extern void (__stdcall* glVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
SQEE_API extern void (__stdcall* glVertexAttrib2dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (__stdcall* glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
SQEE_API extern void (__stdcall* glVertexAttrib2fv)(GLuint index, const GLfloat* v);
SQEE_API extern void (__stdcall* glVertexAttrib2s)(GLuint index, GLshort x, GLshort y);
SQEE_API extern void (__stdcall* glVertexAttrib2sv)(GLuint index, const GLshort* v);
SQEE_API extern void (__stdcall* glVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
SQEE_API extern void (__stdcall* glVertexAttrib3dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (__stdcall* glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
SQEE_API extern void (__stdcall* glVertexAttrib3fv)(GLuint index, const GLfloat* v);
SQEE_API extern void (__stdcall* glVertexAttrib3s)(GLuint index, GLshort x, GLshort y, GLshort z);
SQEE_API extern void (__stdcall* glVertexAttrib3sv)(GLuint index, const GLshort* v);
SQEE_API extern void (__stdcall* glVertexAttrib4Nbv)(GLuint index, const GLbyte* v);
SQEE_API extern void (__stdcall* glVertexAttrib4Niv)(GLuint index, const GLint* v);
SQEE_API extern void (__stdcall* glVertexAttrib4Nsv)(GLuint index, const GLshort* v);
SQEE_API extern void (__stdcall* glVertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
SQEE_API extern void (__stdcall* glVertexAttrib4Nubv)(GLuint index, const GLubyte* v);
SQEE_API extern void (__stdcall* glVertexAttrib4Nuiv)(GLuint index, const GLuint* v);
SQEE_API extern void (__stdcall* glVertexAttrib4Nusv)(GLuint index, const GLushort* v);
SQEE_API extern void (__stdcall* glVertexAttrib4bv)(GLuint index, const GLbyte* v);
SQEE_API extern void (__stdcall* glVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
SQEE_API extern void (__stdcall* glVertexAttrib4dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (__stdcall* glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
SQEE_API extern void (__stdcall* glVertexAttrib4fv)(GLuint index, const GLfloat* v);
SQEE_API extern void (__stdcall* glVertexAttrib4iv)(GLuint index, const GLint* v);
SQEE_API extern void (__stdcall* glVertexAttrib4s)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
SQEE_API extern void (__stdcall* glVertexAttrib4sv)(GLuint index, const GLshort* v);
SQEE_API extern void (__stdcall* glVertexAttrib4ubv)(GLuint index, const GLubyte* v);
SQEE_API extern void (__stdcall* glVertexAttrib4uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (__stdcall* glVertexAttrib4usv)(GLuint index, const GLushort* v);
SQEE_API extern void (__stdcall* glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

// OpenGL 2.1
SQEE_API extern void (__stdcall* glUniformMatrix2x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniformMatrix3x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniformMatrix2x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniformMatrix4x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniformMatrix3x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glUniformMatrix4x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

// OpenGL 3.0
SQEE_API extern void (__stdcall* glColorMaski)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
SQEE_API extern void (__stdcall* glGetBooleani_v)(GLenum target, GLuint index, GLboolean* data);
SQEE_API extern void (__stdcall* glGetIntegeri_v)(GLenum target, GLuint index, GLint* data);
SQEE_API extern void (__stdcall* glEnablei)(GLenum target, GLuint index);
SQEE_API extern void (__stdcall* glDisablei)(GLenum target, GLuint index);
SQEE_API extern GLboolean (__stdcall* glIsEnabledi)(GLenum target, GLuint index);
SQEE_API extern void (__stdcall* glBeginTransformFeedback)(GLenum primitiveMode);
SQEE_API extern void (__stdcall* glEndTransformFeedback)();
SQEE_API extern void (__stdcall* glBindBufferRange)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
SQEE_API extern void (__stdcall* glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
SQEE_API extern void (__stdcall* glTransformFeedbackVaryings)(GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode);
SQEE_API extern void (__stdcall* glGetTransformFeedbackVarying)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name);
SQEE_API extern void (__stdcall* glClampColor)(GLenum target, GLenum clamp);
SQEE_API extern void (__stdcall* glBeginConditionalRender)(GLuint id, GLenum mode);
SQEE_API extern void (__stdcall* glEndConditionalRender)();
SQEE_API extern void (__stdcall* glVertexAttribIPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);
SQEE_API extern void (__stdcall* glGetVertexAttribIiv)(GLuint index, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetVertexAttribIuiv)(GLuint index, GLenum pname, GLuint* params);
SQEE_API extern void (__stdcall* glVertexAttribI1i)(GLuint index, GLint x);
SQEE_API extern void (__stdcall* glVertexAttribI2i)(GLuint index, GLint x, GLint y);
SQEE_API extern void (__stdcall* glVertexAttribI3i)(GLuint index, GLint x, GLint y, GLint z);
SQEE_API extern void (__stdcall* glVertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z, GLint w);
SQEE_API extern void (__stdcall* glVertexAttribI1ui)(GLuint index, GLuint x);
SQEE_API extern void (__stdcall* glVertexAttribI2ui)(GLuint index, GLuint x, GLuint y);
SQEE_API extern void (__stdcall* glVertexAttribI3ui)(GLuint index, GLuint x, GLuint y, GLuint z);
SQEE_API extern void (__stdcall* glVertexAttribI4ui)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
SQEE_API extern void (__stdcall* glVertexAttribI1iv)(GLuint index, const GLint* v);
SQEE_API extern void (__stdcall* glVertexAttribI2iv)(GLuint index, const GLint* v);
SQEE_API extern void (__stdcall* glVertexAttribI3iv)(GLuint index, const GLint* v);
SQEE_API extern void (__stdcall* glVertexAttribI4iv)(GLuint index, const GLint* v);
SQEE_API extern void (__stdcall* glVertexAttribI1uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (__stdcall* glVertexAttribI2uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (__stdcall* glVertexAttribI3uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (__stdcall* glVertexAttribI4uiv)(GLuint index, const GLuint* v);
SQEE_API extern void (__stdcall* glVertexAttribI4bv)(GLuint index, const GLbyte* v);
SQEE_API extern void (__stdcall* glVertexAttribI4sv)(GLuint index, const GLshort* v);
SQEE_API extern void (__stdcall* glVertexAttribI4ubv)(GLuint index, const GLubyte* v);
SQEE_API extern void (__stdcall* glVertexAttribI4usv)(GLuint index, const GLushort* v);
SQEE_API extern void (__stdcall* glGetUniformuiv)(GLuint program, GLint location, GLuint* params);
SQEE_API extern void (__stdcall* glBindFragDataLocation)(GLuint program, GLuint color, const GLchar* name);
SQEE_API extern GLint (__stdcall* glGetFragDataLocation)(GLuint program, const GLchar* name);
SQEE_API extern void (__stdcall* glUniform1ui)(GLint location, GLuint v0);
SQEE_API extern void (__stdcall* glUniform2ui)(GLint location, GLuint v0, GLuint v1);
SQEE_API extern void (__stdcall* glUniform3ui)(GLint location, GLuint v0, GLuint v1, GLuint v2);
SQEE_API extern void (__stdcall* glUniform4ui)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
SQEE_API extern void (__stdcall* glUniform1uiv)(GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (__stdcall* glUniform2uiv)(GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (__stdcall* glUniform3uiv)(GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (__stdcall* glUniform4uiv)(GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (__stdcall* glTexParameterIiv)(GLenum target, GLenum pname, const GLint* params);
SQEE_API extern void (__stdcall* glTexParameterIuiv)(GLenum target, GLenum pname, const GLuint* params);
SQEE_API extern void (__stdcall* glGetTexParameterIiv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetTexParameterIuiv)(GLenum target, GLenum pname, GLuint* params);
SQEE_API extern void (__stdcall* glClearBufferiv)(GLenum buffer, GLint drawbuffer, const GLint* value);
SQEE_API extern void (__stdcall* glClearBufferuiv)(GLenum buffer, GLint drawbuffer, const GLuint* value);
SQEE_API extern void (__stdcall* glClearBufferfv)(GLenum buffer, GLint drawbuffer, const GLfloat* value);
SQEE_API extern void (__stdcall* glClearBufferfi)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
SQEE_API extern const GLubyte* (__stdcall* glGetStringi)(GLenum name, GLuint index);
SQEE_API extern GLboolean (__stdcall* glIsRenderbuffer)(GLuint renderbuffer);
SQEE_API extern void (__stdcall* glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
SQEE_API extern void (__stdcall* glDeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
SQEE_API extern void (__stdcall* glGenRenderbuffers)(GLsizei n, GLuint* renderbuffers);
SQEE_API extern void (__stdcall* glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern GLboolean (__stdcall* glIsFramebuffer)(GLuint framebuffer);
SQEE_API extern void (__stdcall* glBindFramebuffer)(GLenum target, GLuint framebuffer);
SQEE_API extern void (__stdcall* glDeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
SQEE_API extern void (__stdcall* glGenFramebuffers)(GLsizei n, GLuint* framebuffers);
SQEE_API extern GLenum (__stdcall* glCheckFramebufferStatus)(GLenum target);
SQEE_API extern void (__stdcall* glFramebufferTexture1D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
SQEE_API extern void (__stdcall* glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
SQEE_API extern void (__stdcall* glFramebufferTexture3D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
SQEE_API extern void (__stdcall* glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
SQEE_API extern void (__stdcall* glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGenerateMipmap)(GLenum target);
SQEE_API extern void (__stdcall* glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
SQEE_API extern void (__stdcall* glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glFramebufferTextureLayer)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
SQEE_API extern void* (__stdcall* glMapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
SQEE_API extern void (__stdcall* glFlushMappedBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length);
SQEE_API extern void (__stdcall* glBindVertexArray)(GLuint array);
SQEE_API extern void (__stdcall* glDeleteVertexArrays)(GLsizei n, const GLuint* arrays);
SQEE_API extern void (__stdcall* glGenVertexArrays)(GLsizei n, GLuint* arrays);
SQEE_API extern GLboolean (__stdcall* glIsVertexArray)(GLuint array);

// OpenGL 3.1
SQEE_API extern void (__stdcall* glDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
SQEE_API extern void (__stdcall* glDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);
SQEE_API extern void (__stdcall* glTexBuffer)(GLenum target, GLenum internalformat, GLuint buffer);
SQEE_API extern void (__stdcall* glPrimitiveRestartIndex)(GLuint index);
SQEE_API extern void (__stdcall* glCopyBufferSubData)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
SQEE_API extern void (__stdcall* glGetUniformIndices)(GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices);
SQEE_API extern void (__stdcall* glGetActiveUniformsiv)(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetActiveUniformName)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformName);
SQEE_API extern GLuint (__stdcall* glGetUniformBlockIndex)(GLuint program, const GLchar* uniformBlockName);
SQEE_API extern void (__stdcall* glGetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetActiveUniformBlockName)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName);
SQEE_API extern void (__stdcall* glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

// OpenGL 3.2
SQEE_API extern void (__stdcall* glDrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex);
SQEE_API extern void (__stdcall* glDrawRangeElementsBaseVertex)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices, GLint basevertex);
SQEE_API extern void (__stdcall* glDrawElementsInstancedBaseVertex)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex);
SQEE_API extern void (__stdcall* glMultiDrawElementsBaseVertex)(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount, const GLint* basevertex);
SQEE_API extern void (__stdcall* glProvokingVertex)(GLenum mode);
SQEE_API extern GLsync (__stdcall* glFenceSync)(GLenum condition, GLbitfield flags);
SQEE_API extern GLboolean (__stdcall* glIsSync)(GLsync sync);
SQEE_API extern void (__stdcall* glDeleteSync)(GLsync sync);
SQEE_API extern GLenum (__stdcall* glClientWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
SQEE_API extern void (__stdcall* glWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
SQEE_API extern void (__stdcall* glGetInteger64v)(GLenum pname, GLint64* data);
SQEE_API extern void (__stdcall* glGetSynciv)(GLsync sync, GLenum pname, GLsizei count, GLsizei* length, GLint* values);
SQEE_API extern void (__stdcall* glGetInteger64i_v)(GLenum target, GLuint index, GLint64* data);
SQEE_API extern void (__stdcall* glGetBufferParameteri64v)(GLenum target, GLenum pname, GLint64* params);
SQEE_API extern void (__stdcall* glFramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level);
SQEE_API extern void (__stdcall* glTexImage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
SQEE_API extern void (__stdcall* glTexImage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
SQEE_API extern void (__stdcall* glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat* val);
SQEE_API extern void (__stdcall* glSampleMaski)(GLuint maskNumber, GLbitfield mask);

// OpenGL 3.3
SQEE_API extern void (__stdcall* glBindFragDataLocationIndexed)(GLuint program, GLuint colorNumber, GLuint index, const GLchar* name);
SQEE_API extern GLint (__stdcall* glGetFragDataIndex)(GLuint program, const GLchar* name);
SQEE_API extern void (__stdcall* glGenSamplers)(GLsizei count, GLuint* samplers);
SQEE_API extern void (__stdcall* glDeleteSamplers)(GLsizei count, const GLuint* samplers);
SQEE_API extern GLboolean (__stdcall* glIsSampler)(GLuint sampler);
SQEE_API extern void (__stdcall* glBindSampler)(GLuint unit, GLuint sampler);
SQEE_API extern void (__stdcall* glSamplerParameteri)(GLuint sampler, GLenum pname, GLint param);
SQEE_API extern void (__stdcall* glSamplerParameteriv)(GLuint sampler, GLenum pname, const GLint* param);
SQEE_API extern void (__stdcall* glSamplerParameterf)(GLuint sampler, GLenum pname, GLfloat param);
SQEE_API extern void (__stdcall* glSamplerParameterfv)(GLuint sampler, GLenum pname, const GLfloat* param);
SQEE_API extern void (__stdcall* glSamplerParameterIiv)(GLuint sampler, GLenum pname, const GLint* param);
SQEE_API extern void (__stdcall* glSamplerParameterIuiv)(GLuint sampler, GLenum pname, const GLuint* param);
SQEE_API extern void (__stdcall* glGetSamplerParameteriv)(GLuint sampler, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetSamplerParameterIiv)(GLuint sampler, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetSamplerParameterfv)(GLuint sampler, GLenum pname, GLfloat* params);
SQEE_API extern void (__stdcall* glGetSamplerParameterIuiv)(GLuint sampler, GLenum pname, GLuint* params);
SQEE_API extern void (__stdcall* glQueryCounter)(GLuint id, GLenum target);
SQEE_API extern void (__stdcall* glGetQueryObjecti64v)(GLuint id, GLenum pname, GLint64* params);
SQEE_API extern void (__stdcall* glGetQueryObjectui64v)(GLuint id, GLenum pname, GLuint64* params);
SQEE_API extern void (__stdcall* glVertexAttribDivisor)(GLuint index, GLuint divisor);
SQEE_API extern void (__stdcall* glVertexAttribP1ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
SQEE_API extern void (__stdcall* glVertexAttribP1uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint* value);
SQEE_API extern void (__stdcall* glVertexAttribP2ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
SQEE_API extern void (__stdcall* glVertexAttribP2uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint* value);
SQEE_API extern void (__stdcall* glVertexAttribP3ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
SQEE_API extern void (__stdcall* glVertexAttribP3uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint* value);
SQEE_API extern void (__stdcall* glVertexAttribP4ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
SQEE_API extern void (__stdcall* glVertexAttribP4uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint* value);
SQEE_API extern void (__stdcall* glVertexP2ui)(GLenum type, GLuint value);
SQEE_API extern void (__stdcall* glVertexP2uiv)(GLenum type, const GLuint* value);
SQEE_API extern void (__stdcall* glVertexP3ui)(GLenum type, GLuint value);
SQEE_API extern void (__stdcall* glVertexP3uiv)(GLenum type, const GLuint* value);
SQEE_API extern void (__stdcall* glVertexP4ui)(GLenum type, GLuint value);
SQEE_API extern void (__stdcall* glVertexP4uiv)(GLenum type, const GLuint* value);
SQEE_API extern void (__stdcall* glTexCoordP1ui)(GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glTexCoordP1uiv)(GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glTexCoordP2ui)(GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glTexCoordP2uiv)(GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glTexCoordP3ui)(GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glTexCoordP3uiv)(GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glTexCoordP4ui)(GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glTexCoordP4uiv)(GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glMultiTexCoordP1ui)(GLenum texture, GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glMultiTexCoordP1uiv)(GLenum texture, GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glMultiTexCoordP2ui)(GLenum texture, GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glMultiTexCoordP2uiv)(GLenum texture, GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glMultiTexCoordP3ui)(GLenum texture, GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glMultiTexCoordP3uiv)(GLenum texture, GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glMultiTexCoordP4ui)(GLenum texture, GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glMultiTexCoordP4uiv)(GLenum texture, GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glNormalP3ui)(GLenum type, GLuint coords);
SQEE_API extern void (__stdcall* glNormalP3uiv)(GLenum type, const GLuint* coords);
SQEE_API extern void (__stdcall* glColorP3ui)(GLenum type, GLuint color);
SQEE_API extern void (__stdcall* glColorP3uiv)(GLenum type, const GLuint* color);
SQEE_API extern void (__stdcall* glColorP4ui)(GLenum type, GLuint color);
SQEE_API extern void (__stdcall* glColorP4uiv)(GLenum type, const GLuint* color);
SQEE_API extern void (__stdcall* glSecondaryColorP3ui)(GLenum type, GLuint color);
SQEE_API extern void (__stdcall* glSecondaryColorP3uiv)(GLenum type, const GLuint* color);

// OpenGL 4.0
SQEE_API extern void (__stdcall* glMinSampleShading)(GLfloat value);
SQEE_API extern void (__stdcall* glBlendEquationi)(GLuint buf, GLenum mode);
SQEE_API extern void (__stdcall* glBlendEquationSeparatei)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
SQEE_API extern void (__stdcall* glBlendFunci)(GLuint buf, GLenum src, GLenum dst);
SQEE_API extern void (__stdcall* glBlendFuncSeparatei)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
SQEE_API extern void (__stdcall* glDrawArraysIndirect)(GLenum mode, const void* indirect);
SQEE_API extern void (__stdcall* glDrawElementsIndirect)(GLenum mode, GLenum type, const void* indirect);
SQEE_API extern void (__stdcall* glUniform1d)(GLint location, GLdouble x);
SQEE_API extern void (__stdcall* glUniform2d)(GLint location, GLdouble x, GLdouble y);
SQEE_API extern void (__stdcall* glUniform3d)(GLint location, GLdouble x, GLdouble y, GLdouble z);
SQEE_API extern void (__stdcall* glUniform4d)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
SQEE_API extern void (__stdcall* glUniform1dv)(GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniform2dv)(GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniform3dv)(GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniform4dv)(GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix2x3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix2x4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix3x2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix3x4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix4x2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glUniformMatrix4x3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glGetUniformdv)(GLuint program, GLint location, GLdouble* params);
SQEE_API extern GLint (__stdcall* glGetSubroutineUniformLocation)(GLuint program, GLenum shadertype, const GLchar* name);
SQEE_API extern GLuint (__stdcall* glGetSubroutineIndex)(GLuint program, GLenum shadertype, const GLchar* name);
SQEE_API extern void (__stdcall* glGetActiveSubroutineUniformiv)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint* values);
SQEE_API extern void (__stdcall* glGetActiveSubroutineUniformName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name);
SQEE_API extern void (__stdcall* glGetActiveSubroutineName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name);
SQEE_API extern void (__stdcall* glUniformSubroutinesuiv)(GLenum shadertype, GLsizei count, const GLuint* indices);
SQEE_API extern void (__stdcall* glGetUniformSubroutineuiv)(GLenum shadertype, GLint location, GLuint* params);
SQEE_API extern void (__stdcall* glGetProgramStageiv)(GLuint program, GLenum shadertype, GLenum pname, GLint* values);
SQEE_API extern void (__stdcall* glPatchParameteri)(GLenum pname, GLint value);
SQEE_API extern void (__stdcall* glPatchParameterfv)(GLenum pname, const GLfloat* values);
SQEE_API extern void (__stdcall* glBindTransformFeedback)(GLenum target, GLuint id);
SQEE_API extern void (__stdcall* glDeleteTransformFeedbacks)(GLsizei n, const GLuint* ids);
SQEE_API extern void (__stdcall* glGenTransformFeedbacks)(GLsizei n, GLuint* ids);
SQEE_API extern GLboolean (__stdcall* glIsTransformFeedback)(GLuint id);
SQEE_API extern void (__stdcall* glPauseTransformFeedback)();
SQEE_API extern void (__stdcall* glResumeTransformFeedback)();
SQEE_API extern void (__stdcall* glDrawTransformFeedback)(GLenum mode, GLuint id);
SQEE_API extern void (__stdcall* glDrawTransformFeedbackStream)(GLenum mode, GLuint id, GLuint stream);
SQEE_API extern void (__stdcall* glBeginQueryIndexed)(GLenum target, GLuint index, GLuint id);
SQEE_API extern void (__stdcall* glEndQueryIndexed)(GLenum target, GLuint index);
SQEE_API extern void (__stdcall* glGetQueryIndexediv)(GLenum target, GLuint index, GLenum pname, GLint* params);

// OpenGL 4.1
SQEE_API extern void (__stdcall* glReleaseShaderCompiler)();
SQEE_API extern void (__stdcall* glShaderBinary)(GLsizei count, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length);
SQEE_API extern void (__stdcall* glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
SQEE_API extern void (__stdcall* glDepthRangef)(GLfloat n, GLfloat f);
SQEE_API extern void (__stdcall* glClearDepthf)(GLfloat d);
SQEE_API extern void (__stdcall* glGetProgramBinary)(GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, void* binary);
SQEE_API extern void (__stdcall* glProgramBinary)(GLuint program, GLenum binaryFormat, const void* binary, GLsizei length);
SQEE_API extern void (__stdcall* glProgramParameteri)(GLuint program, GLenum pname, GLint value);
SQEE_API extern void (__stdcall* glUseProgramStages)(GLuint pipeline, GLbitfield stages, GLuint program);
SQEE_API extern void (__stdcall* glActiveShaderProgram)(GLuint pipeline, GLuint program);
SQEE_API extern GLuint (__stdcall* glCreateShaderProgramv)(GLenum type, GLsizei count, const GLchar* const* strings);
SQEE_API extern void (__stdcall* glBindProgramPipeline)(GLuint pipeline);
SQEE_API extern void (__stdcall* glDeleteProgramPipelines)(GLsizei n, const GLuint* pipelines);
SQEE_API extern void (__stdcall* glGenProgramPipelines)(GLsizei n, GLuint* pipelines);
SQEE_API extern GLboolean (__stdcall* glIsProgramPipeline)(GLuint pipeline);
SQEE_API extern void (__stdcall* glGetProgramPipelineiv)(GLuint pipeline, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glProgramUniform1i)(GLuint program, GLint location, GLint v0);
SQEE_API extern void (__stdcall* glProgramUniform1iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (__stdcall* glProgramUniform1f)(GLuint program, GLint location, GLfloat v0);
SQEE_API extern void (__stdcall* glProgramUniform1fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniform1d)(GLuint program, GLint location, GLdouble v0);
SQEE_API extern void (__stdcall* glProgramUniform1dv)(GLuint program, GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniform1ui)(GLuint program, GLint location, GLuint v0);
SQEE_API extern void (__stdcall* glProgramUniform1uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (__stdcall* glProgramUniform2i)(GLuint program, GLint location, GLint v0, GLint v1);
SQEE_API extern void (__stdcall* glProgramUniform2iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (__stdcall* glProgramUniform2f)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
SQEE_API extern void (__stdcall* glProgramUniform2fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniform2d)(GLuint program, GLint location, GLdouble v0, GLdouble v1);
SQEE_API extern void (__stdcall* glProgramUniform2dv)(GLuint program, GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniform2ui)(GLuint program, GLint location, GLuint v0, GLuint v1);
SQEE_API extern void (__stdcall* glProgramUniform2uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (__stdcall* glProgramUniform3i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
SQEE_API extern void (__stdcall* glProgramUniform3iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (__stdcall* glProgramUniform3f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
SQEE_API extern void (__stdcall* glProgramUniform3fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniform3d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
SQEE_API extern void (__stdcall* glProgramUniform3dv)(GLuint program, GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniform3ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
SQEE_API extern void (__stdcall* glProgramUniform3uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (__stdcall* glProgramUniform4i)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
SQEE_API extern void (__stdcall* glProgramUniform4iv)(GLuint program, GLint location, GLsizei count, const GLint* value);
SQEE_API extern void (__stdcall* glProgramUniform4f)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
SQEE_API extern void (__stdcall* glProgramUniform4fv)(GLuint program, GLint location, GLsizei count, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniform4d)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
SQEE_API extern void (__stdcall* glProgramUniform4dv)(GLuint program, GLint location, GLsizei count, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniform4ui)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
SQEE_API extern void (__stdcall* glProgramUniform4uiv)(GLuint program, GLint location, GLsizei count, const GLuint* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix2x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix3x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix2x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix4x2fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix3x4fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix4x3fv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix2x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix3x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix2x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix4x2dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix3x4dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glProgramUniformMatrix4x3dv)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value);
SQEE_API extern void (__stdcall* glValidateProgramPipeline)(GLuint pipeline);
SQEE_API extern void (__stdcall* glGetProgramPipelineInfoLog)(GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
SQEE_API extern void (__stdcall* glVertexAttribL1d)(GLuint index, GLdouble x);
SQEE_API extern void (__stdcall* glVertexAttribL2d)(GLuint index, GLdouble x, GLdouble y);
SQEE_API extern void (__stdcall* glVertexAttribL3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
SQEE_API extern void (__stdcall* glVertexAttribL4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
SQEE_API extern void (__stdcall* glVertexAttribL1dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (__stdcall* glVertexAttribL2dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (__stdcall* glVertexAttribL3dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (__stdcall* glVertexAttribL4dv)(GLuint index, const GLdouble* v);
SQEE_API extern void (__stdcall* glVertexAttribLPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);
SQEE_API extern void (__stdcall* glGetVertexAttribLdv)(GLuint index, GLenum pname, GLdouble* params);
SQEE_API extern void (__stdcall* glViewportArrayv)(GLuint first, GLsizei count, const GLfloat* v);
SQEE_API extern void (__stdcall* glViewportIndexedf)(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
SQEE_API extern void (__stdcall* glViewportIndexedfv)(GLuint index, const GLfloat* v);
SQEE_API extern void (__stdcall* glScissorArrayv)(GLuint first, GLsizei count, const GLint* v);
SQEE_API extern void (__stdcall* glScissorIndexed)(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glScissorIndexedv)(GLuint index, const GLint* v);
SQEE_API extern void (__stdcall* glDepthRangeArrayv)(GLuint first, GLsizei count, const GLdouble* v);
SQEE_API extern void (__stdcall* glDepthRangeIndexed)(GLuint index, GLdouble n, GLdouble f);
SQEE_API extern void (__stdcall* glGetFloati_v)(GLenum target, GLuint index, GLfloat* data);
SQEE_API extern void (__stdcall* glGetDoublei_v)(GLenum target, GLuint index, GLdouble* data);

// OpenGL 4.2
SQEE_API extern void (__stdcall* glDrawArraysInstancedBaseInstance)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
SQEE_API extern void (__stdcall* glDrawElementsInstancedBaseInstance)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance);
SQEE_API extern void (__stdcall* glDrawElementsInstancedBaseVertexBaseInstance)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
SQEE_API extern void (__stdcall* glGetInternalformativ)(GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint* params);
SQEE_API extern void (__stdcall* glGetActiveAtomicCounterBufferiv)(GLuint program, GLuint bufferIndex, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glBindImageTexture)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
SQEE_API extern void (__stdcall* glMemoryBarrier)(GLbitfield barriers);
SQEE_API extern void (__stdcall* glTexStorage1D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
SQEE_API extern void (__stdcall* glTexStorage2D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glTexStorage3D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
SQEE_API extern void (__stdcall* glDrawTransformFeedbackInstanced)(GLenum mode, GLuint id, GLsizei instancecount);
SQEE_API extern void (__stdcall* glDrawTransformFeedbackStreamInstanced)(GLenum mode, GLuint id, GLuint stream, GLsizei instancecount);

// OpenGL 4.3
SQEE_API extern void (__stdcall* glClearBufferData)(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void* data);
SQEE_API extern void (__stdcall* glClearBufferSubData)(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);
SQEE_API extern void (__stdcall* glDispatchCompute)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
SQEE_API extern void (__stdcall* glDispatchComputeIndirect)(GLintptr indirect);
SQEE_API extern void (__stdcall* glCopyImageSubData)(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
SQEE_API extern void (__stdcall* glFramebufferParameteri)(GLenum target, GLenum pname, GLint param);
SQEE_API extern void (__stdcall* glGetFramebufferParameteriv)(GLenum target, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetInternalformati64v)(GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint64* params);
SQEE_API extern void (__stdcall* glInvalidateTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
SQEE_API extern void (__stdcall* glInvalidateTexImage)(GLuint texture, GLint level);
SQEE_API extern void (__stdcall* glInvalidateBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr length);
SQEE_API extern void (__stdcall* glInvalidateBufferData)(GLuint buffer);
SQEE_API extern void (__stdcall* glInvalidateFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum* attachments);
SQEE_API extern void (__stdcall* glInvalidateSubFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glMultiDrawArraysIndirect)(GLenum mode, const void* indirect, GLsizei drawcount, GLsizei stride);
SQEE_API extern void (__stdcall* glMultiDrawElementsIndirect)(GLenum mode, GLenum type, const void* indirect, GLsizei drawcount, GLsizei stride);
SQEE_API extern void (__stdcall* glGetProgramInterfaceiv)(GLuint program, GLenum programInterface, GLenum pname, GLint* params);
SQEE_API extern GLuint (__stdcall* glGetProgramResourceIndex)(GLuint program, GLenum programInterface, const GLchar* name);
SQEE_API extern void (__stdcall* glGetProgramResourceName)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name);
SQEE_API extern void (__stdcall* glGetProgramResourceiv)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei count, GLsizei* length, GLint* params);
SQEE_API extern GLint (__stdcall* glGetProgramResourceLocation)(GLuint program, GLenum programInterface, const GLchar* name);
SQEE_API extern GLint (__stdcall* glGetProgramResourceLocationIndex)(GLuint program, GLenum programInterface, const GLchar* name);
SQEE_API extern void (__stdcall* glShaderStorageBlockBinding)(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
SQEE_API extern void (__stdcall* glTexBufferRange)(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
SQEE_API extern void (__stdcall* glTexStorage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
SQEE_API extern void (__stdcall* glTexStorage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
SQEE_API extern void (__stdcall* glTextureView)(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
SQEE_API extern void (__stdcall* glBindVertexBuffer)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
SQEE_API extern void (__stdcall* glVertexAttribFormat)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
SQEE_API extern void (__stdcall* glVertexAttribIFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
SQEE_API extern void (__stdcall* glVertexAttribLFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
SQEE_API extern void (__stdcall* glVertexAttribBinding)(GLuint attribindex, GLuint bindingindex);
SQEE_API extern void (__stdcall* glVertexBindingDivisor)(GLuint bindingindex, GLuint divisor);
SQEE_API extern void (__stdcall* glDebugMessageControl)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
SQEE_API extern void (__stdcall* glDebugMessageInsert)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf);
SQEE_API extern void (__stdcall* glDebugMessageCallback)(GLDebugCallback callback, const void* userParam);
SQEE_API extern GLuint (__stdcall* glGetDebugMessageLog)(GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog);
SQEE_API extern void (__stdcall* glPushDebugGroup)(GLenum source, GLuint id, GLsizei length, const GLchar* message);
SQEE_API extern void (__stdcall* glPopDebugGroup)();
SQEE_API extern void (__stdcall* glObjectLabel)(GLenum identifier, GLuint name, GLsizei length, const GLchar* label);
SQEE_API extern void (__stdcall* glGetObjectLabel)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei* length, GLchar* label);
SQEE_API extern void (__stdcall* glObjectPtrLabel)(const void* ptr, GLsizei length, const GLchar* label);
SQEE_API extern void (__stdcall* glGetObjectPtrLabel)(const void* ptr, GLsizei bufSize, GLsizei* length, GLchar* label);
SQEE_API extern void (__stdcall* glGetPointerv)(GLenum pname, void** params);

// OpenGL 4.4
SQEE_API extern void (__stdcall* glBufferStorage)(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);
SQEE_API extern void (__stdcall* glClearTexImage)(GLuint texture, GLint level, GLenum format, GLenum type, const void* data);
SQEE_API extern void (__stdcall* glClearTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* data);
SQEE_API extern void (__stdcall* glBindBuffersBase)(GLenum target, GLuint first, GLsizei count, const GLuint* buffers);
SQEE_API extern void (__stdcall* glBindBuffersRange)(GLenum target, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizeiptr* sizes);
SQEE_API extern void (__stdcall* glBindTextures)(GLuint first, GLsizei count, const GLuint* textures);
SQEE_API extern void (__stdcall* glBindSamplers)(GLuint first, GLsizei count, const GLuint* samplers);
SQEE_API extern void (__stdcall* glBindImageTextures)(GLuint first, GLsizei count, const GLuint* textures);
SQEE_API extern void (__stdcall* glBindVertexBuffers)(GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides);

// OpenGL 4.5
SQEE_API extern void (__stdcall* glClipControl)(GLenum origin, GLenum depth);
SQEE_API extern void (__stdcall* glCreateTransformFeedbacks)(GLsizei n, GLuint* ids);
SQEE_API extern void (__stdcall* glTransformFeedbackBufferBase)(GLuint xfb, GLuint index, GLuint buffer);
SQEE_API extern void (__stdcall* glTransformFeedbackBufferRange)(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
SQEE_API extern void (__stdcall* glGetTransformFeedbackiv)(GLuint xfb, GLenum pname, GLint* param);
SQEE_API extern void (__stdcall* glGetTransformFeedbacki_v)(GLuint xfb, GLenum pname, GLuint index, GLint* param);
SQEE_API extern void (__stdcall* glGetTransformFeedbacki64_v)(GLuint xfb, GLenum pname, GLuint index, GLint64* param);
SQEE_API extern void (__stdcall* glCreateBuffers)(GLsizei n, GLuint* buffers);
SQEE_API extern void (__stdcall* glNamedBufferStorage)(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
SQEE_API extern void (__stdcall* glNamedBufferData)(GLuint buffer, GLsizeiptr size, const void* data, GLenum usage);
SQEE_API extern void (__stdcall* glNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
SQEE_API extern void (__stdcall* glCopyNamedBufferSubData)(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
SQEE_API extern void (__stdcall* glClearNamedBufferData)(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data);
SQEE_API extern void (__stdcall* glClearNamedBufferSubData)(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);
SQEE_API extern void* (__stdcall* glMapNamedBuffer)(GLuint buffer, GLenum access);
SQEE_API extern void* (__stdcall* glMapNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
SQEE_API extern GLboolean (__stdcall* glUnmapNamedBuffer)(GLuint buffer);
SQEE_API extern void (__stdcall* glFlushMappedNamedBufferRange)(GLuint buffer, GLintptr offset, GLsizeiptr length);
SQEE_API extern void (__stdcall* glGetNamedBufferParameteriv)(GLuint buffer, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetNamedBufferParameteri64v)(GLuint buffer, GLenum pname, GLint64* params);
SQEE_API extern void (__stdcall* glGetNamedBufferPointerv)(GLuint buffer, GLenum pname, void** params);
SQEE_API extern void (__stdcall* glGetNamedBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);
SQEE_API extern void (__stdcall* glCreateFramebuffers)(GLsizei n, GLuint* framebuffers);
SQEE_API extern void (__stdcall* glNamedFramebufferRenderbuffer)(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
SQEE_API extern void (__stdcall* glNamedFramebufferParameteri)(GLuint framebuffer, GLenum pname, GLint param);
SQEE_API extern void (__stdcall* glNamedFramebufferTexture)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
SQEE_API extern void (__stdcall* glNamedFramebufferTextureLayer)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
SQEE_API extern void (__stdcall* glNamedFramebufferDrawBuffer)(GLuint framebuffer, GLenum buf);
SQEE_API extern void (__stdcall* glNamedFramebufferDrawBuffers)(GLuint framebuffer, GLsizei n, const GLenum* bufs);
SQEE_API extern void (__stdcall* glNamedFramebufferReadBuffer)(GLuint framebuffer, GLenum src);
SQEE_API extern void (__stdcall* glInvalidateNamedFramebufferData)(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments);
SQEE_API extern void (__stdcall* glInvalidateNamedFramebufferSubData)(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glClearNamedFramebufferiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value);
SQEE_API extern void (__stdcall* glClearNamedFramebufferuiv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value);
SQEE_API extern void (__stdcall* glClearNamedFramebufferfv)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);
SQEE_API extern void (__stdcall* glClearNamedFramebufferfi)(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
SQEE_API extern void (__stdcall* glBlitNamedFramebuffer)(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
SQEE_API extern GLenum (__stdcall* glCheckNamedFramebufferStatus)(GLuint framebuffer, GLenum target);
SQEE_API extern void (__stdcall* glGetNamedFramebufferParameteriv)(GLuint framebuffer, GLenum pname, GLint* param);
SQEE_API extern void (__stdcall* glGetNamedFramebufferAttachmentParameteriv)(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glCreateRenderbuffers)(GLsizei n, GLuint* renderbuffers);
SQEE_API extern void (__stdcall* glNamedRenderbufferStorage)(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glNamedRenderbufferStorageMultisample)(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glGetNamedRenderbufferParameteriv)(GLuint renderbuffer, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glCreateTextures)(GLenum target, GLsizei n, GLuint* textures);
SQEE_API extern void (__stdcall* glTextureBuffer)(GLuint texture, GLenum internalformat, GLuint buffer);
SQEE_API extern void (__stdcall* glTextureBufferRange)(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
SQEE_API extern void (__stdcall* glTextureStorage1D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
SQEE_API extern void (__stdcall* glTextureStorage2D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glTextureStorage3D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
SQEE_API extern void (__stdcall* glTextureStorage2DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
SQEE_API extern void (__stdcall* glTextureStorage3DMultisample)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
SQEE_API extern void (__stdcall* glTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
SQEE_API extern void (__stdcall* glCompressedTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glCompressedTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glCompressedTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
SQEE_API extern void (__stdcall* glCopyTextureSubImage1D)(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
SQEE_API extern void (__stdcall* glCopyTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glCopyTextureSubImage3D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
SQEE_API extern void (__stdcall* glTextureParameterf)(GLuint texture, GLenum pname, GLfloat param);
SQEE_API extern void (__stdcall* glTextureParameterfv)(GLuint texture, GLenum pname, const GLfloat* param);
SQEE_API extern void (__stdcall* glTextureParameteri)(GLuint texture, GLenum pname, GLint param);
SQEE_API extern void (__stdcall* glTextureParameterIiv)(GLuint texture, GLenum pname, const GLint* params);
SQEE_API extern void (__stdcall* glTextureParameterIuiv)(GLuint texture, GLenum pname, const GLuint* params);
SQEE_API extern void (__stdcall* glTextureParameteriv)(GLuint texture, GLenum pname, const GLint* param);
SQEE_API extern void (__stdcall* glGenerateTextureMipmap)(GLuint texture);
SQEE_API extern void (__stdcall* glBindTextureUnit)(GLuint unit, GLuint texture);
SQEE_API extern void (__stdcall* glGetTextureImage)(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
SQEE_API extern void (__stdcall* glGetCompressedTextureImage)(GLuint texture, GLint level, GLsizei bufSize, void* pixels);
SQEE_API extern void (__stdcall* glGetTextureLevelParameterfv)(GLuint texture, GLint level, GLenum pname, GLfloat* params);
SQEE_API extern void (__stdcall* glGetTextureLevelParameteriv)(GLuint texture, GLint level, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetTextureParameterfv)(GLuint texture, GLenum pname, GLfloat* params);
SQEE_API extern void (__stdcall* glGetTextureParameterIiv)(GLuint texture, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glGetTextureParameterIuiv)(GLuint texture, GLenum pname, GLuint* params);
SQEE_API extern void (__stdcall* glGetTextureParameteriv)(GLuint texture, GLenum pname, GLint* params);
SQEE_API extern void (__stdcall* glCreateVertexArrays)(GLsizei n, GLuint* arrays);
SQEE_API extern void (__stdcall* glDisableVertexArrayAttrib)(GLuint vaobj, GLuint index);
SQEE_API extern void (__stdcall* glEnableVertexArrayAttrib)(GLuint vaobj, GLuint index);
SQEE_API extern void (__stdcall* glVertexArrayElementBuffer)(GLuint vaobj, GLuint buffer);
SQEE_API extern void (__stdcall* glVertexArrayVertexBuffer)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
SQEE_API extern void (__stdcall* glVertexArrayVertexBuffers)(GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides);
SQEE_API extern void (__stdcall* glVertexArrayAttribBinding)(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
SQEE_API extern void (__stdcall* glVertexArrayAttribFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
SQEE_API extern void (__stdcall* glVertexArrayAttribIFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
SQEE_API extern void (__stdcall* glVertexArrayAttribLFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
SQEE_API extern void (__stdcall* glVertexArrayBindingDivisor)(GLuint vaobj, GLuint bindingindex, GLuint divisor);
SQEE_API extern void (__stdcall* glGetVertexArrayiv)(GLuint vaobj, GLenum pname, GLint* param);
SQEE_API extern void (__stdcall* glGetVertexArrayIndexediv)(GLuint vaobj, GLuint index, GLenum pname, GLint* param);
SQEE_API extern void (__stdcall* glGetVertexArrayIndexed64iv)(GLuint vaobj, GLuint index, GLenum pname, GLint64* param);
SQEE_API extern void (__stdcall* glCreateSamplers)(GLsizei n, GLuint* samplers);
SQEE_API extern void (__stdcall* glCreateProgramPipelines)(GLsizei n, GLuint* pipelines);
SQEE_API extern void (__stdcall* glCreateQueries)(GLenum target, GLsizei n, GLuint* ids);
SQEE_API extern void (__stdcall* glGetQueryBufferObjecti64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
SQEE_API extern void (__stdcall* glGetQueryBufferObjectiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
SQEE_API extern void (__stdcall* glGetQueryBufferObjectui64v)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
SQEE_API extern void (__stdcall* glGetQueryBufferObjectuiv)(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
SQEE_API extern void (__stdcall* glMemoryBarrierByRegion)(GLbitfield barriers);
SQEE_API extern void (__stdcall* glGetTextureSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
SQEE_API extern void (__stdcall* glGetCompressedTextureSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void* pixels);
SQEE_API extern GLenum (__stdcall* glGetGraphicsResetStatus)();
SQEE_API extern void (__stdcall* glGetnCompressedTexImage)(GLenum target, GLint lod, GLsizei bufSize, void* pixels);
SQEE_API extern void (__stdcall* glGetnTexImage)(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
SQEE_API extern void (__stdcall* glGetnUniformdv)(GLuint program, GLint location, GLsizei bufSize, GLdouble* params);
SQEE_API extern void (__stdcall* glGetnUniformfv)(GLuint program, GLint location, GLsizei bufSize, GLfloat* params);
SQEE_API extern void (__stdcall* glGetnUniformiv)(GLuint program, GLint location, GLsizei bufSize, GLint* params);
SQEE_API extern void (__stdcall* glGetnUniformuiv)(GLuint program, GLint location, GLsizei bufSize, GLuint* params);
SQEE_API extern void (__stdcall* glReadnPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void* data);
SQEE_API extern void (__stdcall* glGetnMapdv)(GLenum target, GLenum query, GLsizei bufSize, GLdouble* v);
SQEE_API extern void (__stdcall* glGetnMapfv)(GLenum target, GLenum query, GLsizei bufSize, GLfloat* v);
SQEE_API extern void (__stdcall* glGetnMapiv)(GLenum target, GLenum query, GLsizei bufSize, GLint* v);
SQEE_API extern void (__stdcall* glGetnPixelMapfv)(GLenum map, GLsizei bufSize, GLfloat* values);
SQEE_API extern void (__stdcall* glGetnPixelMapuiv)(GLenum map, GLsizei bufSize, GLuint* values);
SQEE_API extern void (__stdcall* glGetnPixelMapusv)(GLenum map, GLsizei bufSize, GLushort* values);
SQEE_API extern void (__stdcall* glGetnPolygonStipple)(GLsizei bufSize, GLubyte* pattern);
SQEE_API extern void (__stdcall* glGetnColorTable)(GLenum target, GLenum format, GLenum type, GLsizei bufSize, void* table);
SQEE_API extern void (__stdcall* glGetnConvolutionFilter)(GLenum target, GLenum format, GLenum type, GLsizei bufSize, void* image);
SQEE_API extern void (__stdcall* glGetnSeparableFilter)(GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, void* row, GLsizei columnBufSize, void* column, void* span);
SQEE_API extern void (__stdcall* glGetnHistogram)(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void* values);
SQEE_API extern void (__stdcall* glGetnMinmax)(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void* values);
SQEE_API extern void (__stdcall* glTextureBarrier)();

//============================================================================//

} // namespace sq::gl::detail

namespace sq::gl {

//============================================================================//

// OpenGL 1.0
inline void /*gl*/CullFace(GLenum mode) { detail::glCullFace(mode); }
inline void /*gl*/FrontFace(GLenum mode) { detail::glFrontFace(mode); }
inline void /*gl*/Hint(GLenum target, GLenum mode) { detail::glHint(target, mode); }
inline void /*gl*/LineWidth(GLfloat width) { detail::glLineWidth(width); }
inline void /*gl*/PointSize(GLfloat size) { detail::glPointSize(size); }
inline void /*gl*/PolygonMode(GLenum face, GLenum mode) { detail::glPolygonMode(face, mode); }
inline void /*gl*/Scissor(GLint x, GLint y, GLsizei width, GLsizei height) { detail::glScissor(x, y, width, height); }
inline void /*gl*/TexParameterf(GLenum target, GLenum pname, GLfloat param) { detail::glTexParameterf(target, pname, param); }
inline void /*gl*/TexParameterfv(GLenum target, GLenum pname, const GLfloat* params) { detail::glTexParameterfv(target, pname, params); }
inline void /*gl*/TexParameteri(GLenum target, GLenum pname, GLint param) { detail::glTexParameteri(target, pname, param); }
inline void /*gl*/TexParameteriv(GLenum target, GLenum pname, const GLint* params) { detail::glTexParameteriv(target, pname, params); }
inline void /*gl*/TexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* pixels) { detail::glTexImage1D(target, level, internalformat, width, border, format, type, pixels); }
inline void /*gl*/TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) { detail::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels); }
inline void /*gl*/DrawBuffer(GLenum buf) { detail::glDrawBuffer(buf); }
inline void /*gl*/Clear(GLbitfield mask) { detail::glClear(mask); }
inline void /*gl*/ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) { detail::glClearColor(red, green, blue, alpha); }
inline void /*gl*/ClearStencil(GLint s) { detail::glClearStencil(s); }
inline void /*gl*/ClearDepth(GLdouble depth) { detail::glClearDepth(depth); }
inline void /*gl*/StencilMask(GLuint mask) { detail::glStencilMask(mask); }
inline void /*gl*/ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) { detail::glColorMask(red, green, blue, alpha); }
inline void /*gl*/DepthMask(GLboolean flag) { detail::glDepthMask(flag); }
inline void /*gl*/Disable(GLenum cap) { detail::glDisable(cap); }
inline void /*gl*/Enable(GLenum cap) { detail::glEnable(cap); }
inline void /*gl*/Finish() { detail::glFinish(); }
inline void /*gl*/Flush() { detail::glFlush(); }
inline void /*gl*/BlendFunc(GLenum sfactor, GLenum dfactor) { detail::glBlendFunc(sfactor, dfactor); }
inline void /*gl*/LogicOp(GLenum opcode) { detail::glLogicOp(opcode); }
inline void /*gl*/StencilFunc(GLenum func, GLint ref, GLuint mask) { detail::glStencilFunc(func, ref, mask); }
inline void /*gl*/StencilOp(GLenum fail, GLenum zfail, GLenum zpass) { detail::glStencilOp(fail, zfail, zpass); }
inline void /*gl*/DepthFunc(GLenum func) { detail::glDepthFunc(func); }
inline void /*gl*/PixelStoref(GLenum pname, GLfloat param) { detail::glPixelStoref(pname, param); }
inline void /*gl*/PixelStorei(GLenum pname, GLint param) { detail::glPixelStorei(pname, param); }
inline void /*gl*/ReadBuffer(GLenum src) { detail::glReadBuffer(src); }
inline void /*gl*/ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) { detail::glReadPixels(x, y, width, height, format, type, pixels); }
inline void /*gl*/GetBooleanv(GLenum pname, GLboolean* data) { detail::glGetBooleanv(pname, data); }
inline void /*gl*/GetDoublev(GLenum pname, GLdouble* data) { detail::glGetDoublev(pname, data); }
inline GLenum /*gl*/GetError() { return detail::glGetError(); }
inline void /*gl*/GetFloatv(GLenum pname, GLfloat* data) { detail::glGetFloatv(pname, data); }
inline void /*gl*/GetIntegerv(GLenum pname, GLint* data) { detail::glGetIntegerv(pname, data); }
inline const GLubyte* /*gl*/GetString(GLenum name) { return detail::glGetString(name); }
inline void /*gl*/GetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels) { detail::glGetTexImage(target, level, format, type, pixels); }
inline void /*gl*/GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params) { detail::glGetTexParameterfv(target, pname, params); }
inline void /*gl*/GetTexParameteriv(GLenum target, GLenum pname, GLint* params) { detail::glGetTexParameteriv(target, pname, params); }
inline void /*gl*/GetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat* params) { detail::glGetTexLevelParameterfv(target, level, pname, params); }
inline void /*gl*/GetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params) { detail::glGetTexLevelParameteriv(target, level, pname, params); }
inline GLboolean /*gl*/IsEnabled(GLenum cap) { return detail::glIsEnabled(cap); }
inline void /*gl*/DepthRange(GLdouble n, GLdouble f) { detail::glDepthRange(n, f); }
inline void /*gl*/Viewport(GLint x, GLint y, GLsizei width, GLsizei height) { detail::glViewport(x, y, width, height); }

// OpenGL 1.1
inline void /*gl*/DrawArrays(GLenum mode, GLint first, GLsizei count) { detail::glDrawArrays(mode, first, count); }
inline void /*gl*/DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) { detail::glDrawElements(mode, count, type, indices); }
inline void /*gl*/PolygonOffset(GLfloat factor, GLfloat units) { detail::glPolygonOffset(factor, units); }
inline void /*gl*/CopyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border) { detail::glCopyTexImage1D(target, level, internalformat, x, y, width, border); }
inline void /*gl*/CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) { detail::glCopyTexImage2D(target, level, internalformat, x, y, width, height, border); }
inline void /*gl*/CopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) { detail::glCopyTexSubImage1D(target, level, xoffset, x, y, width); }
inline void /*gl*/CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) { detail::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height); }
inline void /*gl*/TexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels) { detail::glTexSubImage1D(target, level, xoffset, width, format, type, pixels); }
inline void /*gl*/TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) { detail::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels); }
inline void /*gl*/BindTexture(GLenum target, GLuint texture) { detail::glBindTexture(target, texture); }
inline void /*gl*/DeleteTextures(GLsizei n, const GLuint* textures) { detail::glDeleteTextures(n, textures); }
inline void /*gl*/GenTextures(GLsizei n, GLuint* textures) { detail::glGenTextures(n, textures); }
inline GLboolean /*gl*/IsTexture(GLuint texture) { return detail::glIsTexture(texture); }

// OpenGL 1.2
inline void /*gl*/DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices) { detail::glDrawRangeElements(mode, start, end, count, type, indices); }
inline void /*gl*/TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels) { detail::glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels); }
inline void /*gl*/TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels) { detail::glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels); }
inline void /*gl*/CopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) { detail::glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height); }

// OpenGL 1.3
inline void /*gl*/ActiveTexture(GLenum texture) { detail::glActiveTexture(texture); }
inline void /*gl*/SampleCoverage(GLfloat value, GLboolean invert) { detail::glSampleCoverage(value, invert); }
inline void /*gl*/CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data) { detail::glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data); }
inline void /*gl*/CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data) { detail::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data); }
inline void /*gl*/CompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void* data) { detail::glCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data); }
inline void /*gl*/CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data) { detail::glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data); }
inline void /*gl*/CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) { detail::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data); }
inline void /*gl*/CompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data) { detail::glCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data); }
inline void /*gl*/GetCompressedTexImage(GLenum target, GLint level, void* img) { detail::glGetCompressedTexImage(target, level, img); }

// OpenGL 1.4
inline void /*gl*/BlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) { detail::glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha); }
inline void /*gl*/MultiDrawArrays(GLenum mode, const GLint* first, const GLsizei* count, GLsizei drawcount) { detail::glMultiDrawArrays(mode, first, count, drawcount); }
inline void /*gl*/MultiDrawElements(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount) { detail::glMultiDrawElements(mode, count, type, indices, drawcount); }
inline void /*gl*/PointParameterf(GLenum pname, GLfloat param) { detail::glPointParameterf(pname, param); }
inline void /*gl*/PointParameterfv(GLenum pname, const GLfloat* params) { detail::glPointParameterfv(pname, params); }
inline void /*gl*/PointParameteri(GLenum pname, GLint param) { detail::glPointParameteri(pname, param); }
inline void /*gl*/PointParameteriv(GLenum pname, const GLint* params) { detail::glPointParameteriv(pname, params); }
inline void /*gl*/BlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) { detail::glBlendColor(red, green, blue, alpha); }
inline void /*gl*/BlendEquation(GLenum mode) { detail::glBlendEquation(mode); }

// OpenGL 1.5
inline void /*gl*/GenQueries(GLsizei n, GLuint* ids) { detail::glGenQueries(n, ids); }
inline void /*gl*/DeleteQueries(GLsizei n, const GLuint* ids) { detail::glDeleteQueries(n, ids); }
inline GLboolean /*gl*/IsQuery(GLuint id) { return detail::glIsQuery(id); }
inline void /*gl*/BeginQuery(GLenum target, GLuint id) { detail::glBeginQuery(target, id); }
inline void /*gl*/EndQuery(GLenum target) { detail::glEndQuery(target); }
inline void /*gl*/GetQueryiv(GLenum target, GLenum pname, GLint* params) { detail::glGetQueryiv(target, pname, params); }
inline void /*gl*/GetQueryObjectiv(GLuint id, GLenum pname, GLint* params) { detail::glGetQueryObjectiv(id, pname, params); }
inline void /*gl*/GetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params) { detail::glGetQueryObjectuiv(id, pname, params); }
inline void /*gl*/BindBuffer(GLenum target, GLuint buffer) { detail::glBindBuffer(target, buffer); }
inline void /*gl*/DeleteBuffers(GLsizei n, const GLuint* buffers) { detail::glDeleteBuffers(n, buffers); }
inline void /*gl*/GenBuffers(GLsizei n, GLuint* buffers) { detail::glGenBuffers(n, buffers); }
inline GLboolean /*gl*/IsBuffer(GLuint buffer) { return detail::glIsBuffer(buffer); }
inline void /*gl*/BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) { detail::glBufferData(target, size, data, usage); }
inline void /*gl*/BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) { detail::glBufferSubData(target, offset, size, data); }
inline void /*gl*/GetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, void* data) { detail::glGetBufferSubData(target, offset, size, data); }
inline void* /*gl*/MapBuffer(GLenum target, GLenum access) { return detail::glMapBuffer(target, access); }
inline GLboolean /*gl*/UnmapBuffer(GLenum target) { return detail::glUnmapBuffer(target); }
inline void /*gl*/GetBufferParameteriv(GLenum target, GLenum pname, GLint* params) { detail::glGetBufferParameteriv(target, pname, params); }
inline void /*gl*/GetBufferPointerv(GLenum target, GLenum pname, void** params) { detail::glGetBufferPointerv(target, pname, params); }

// OpenGL 2.0
inline void /*gl*/BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) { detail::glBlendEquationSeparate(modeRGB, modeAlpha); }
inline void /*gl*/DrawBuffers(GLsizei n, const GLenum* bufs) { detail::glDrawBuffers(n, bufs); }
inline void /*gl*/StencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) { detail::glStencilOpSeparate(face, sfail, dpfail, dppass); }
inline void /*gl*/StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) { detail::glStencilFuncSeparate(face, func, ref, mask); }
inline void /*gl*/StencilMaskSeparate(GLenum face, GLuint mask) { detail::glStencilMaskSeparate(face, mask); }
inline void /*gl*/AttachShader(GLuint program, GLuint shader) { detail::glAttachShader(program, shader); }
inline void /*gl*/BindAttribLocation(GLuint program, GLuint index, const GLchar* name) { detail::glBindAttribLocation(program, index, name); }
inline void /*gl*/CompileShader(GLuint shader) { detail::glCompileShader(shader); }
inline GLuint /*gl*/CreateProgram() { return detail::glCreateProgram(); }
inline GLuint /*gl*/CreateShader(GLenum type) { return detail::glCreateShader(type); }
inline void /*gl*/DeleteProgram(GLuint program) { detail::glDeleteProgram(program); }
inline void /*gl*/DeleteShader(GLuint shader) { detail::glDeleteShader(shader); }
inline void /*gl*/DetachShader(GLuint program, GLuint shader) { detail::glDetachShader(program, shader); }
inline void /*gl*/DisableVertexAttribArray(GLuint index) { detail::glDisableVertexAttribArray(index); }
inline void /*gl*/EnableVertexAttribArray(GLuint index) { detail::glEnableVertexAttribArray(index); }
inline void /*gl*/GetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) { detail::glGetActiveAttrib(program, index, bufSize, length, size, type, name); }
inline void /*gl*/GetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) { detail::glGetActiveUniform(program, index, bufSize, length, size, type, name); }
inline void /*gl*/GetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders) { detail::glGetAttachedShaders(program, maxCount, count, shaders); }
inline GLint /*gl*/GetAttribLocation(GLuint program, const GLchar* name) { return detail::glGetAttribLocation(program, name); }
inline void /*gl*/GetProgramiv(GLuint program, GLenum pname, GLint* params) { detail::glGetProgramiv(program, pname, params); }
inline void /*gl*/GetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog) { detail::glGetProgramInfoLog(program, bufSize, length, infoLog); }
inline void /*gl*/GetShaderiv(GLuint shader, GLenum pname, GLint* params) { detail::glGetShaderiv(shader, pname, params); }
inline void /*gl*/GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog) { detail::glGetShaderInfoLog(shader, bufSize, length, infoLog); }
inline void /*gl*/GetShaderSource(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source) { detail::glGetShaderSource(shader, bufSize, length, source); }
inline GLint /*gl*/GetUniformLocation(GLuint program, const GLchar* name) { return detail::glGetUniformLocation(program, name); }
inline void /*gl*/GetUniformfv(GLuint program, GLint location, GLfloat* params) { detail::glGetUniformfv(program, location, params); }
inline void /*gl*/GetUniformiv(GLuint program, GLint location, GLint* params) { detail::glGetUniformiv(program, location, params); }
inline void /*gl*/GetVertexAttribdv(GLuint index, GLenum pname, GLdouble* params) { detail::glGetVertexAttribdv(index, pname, params); }
inline void /*gl*/GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params) { detail::glGetVertexAttribfv(index, pname, params); }
inline void /*gl*/GetVertexAttribiv(GLuint index, GLenum pname, GLint* params) { detail::glGetVertexAttribiv(index, pname, params); }
inline void /*gl*/GetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer) { detail::glGetVertexAttribPointerv(index, pname, pointer); }
inline GLboolean /*gl*/IsProgram(GLuint program) { return detail::glIsProgram(program); }
inline GLboolean /*gl*/IsShader(GLuint shader) { return detail::glIsShader(shader); }
inline void /*gl*/LinkProgram(GLuint program) { detail::glLinkProgram(program); }
inline void /*gl*/ShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length) { detail::glShaderSource(shader, count, string, length); }
inline void /*gl*/UseProgram(GLuint program) { detail::glUseProgram(program); }
inline void /*gl*/Uniform1f(GLint location, GLfloat v0) { detail::glUniform1f(location, v0); }
inline void /*gl*/Uniform2f(GLint location, GLfloat v0, GLfloat v1) { detail::glUniform2f(location, v0, v1); }
inline void /*gl*/Uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) { detail::glUniform3f(location, v0, v1, v2); }
inline void /*gl*/Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { detail::glUniform4f(location, v0, v1, v2, v3); }
inline void /*gl*/Uniform1i(GLint location, GLint v0) { detail::glUniform1i(location, v0); }
inline void /*gl*/Uniform2i(GLint location, GLint v0, GLint v1) { detail::glUniform2i(location, v0, v1); }
inline void /*gl*/Uniform3i(GLint location, GLint v0, GLint v1, GLint v2) { detail::glUniform3i(location, v0, v1, v2); }
inline void /*gl*/Uniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) { detail::glUniform4i(location, v0, v1, v2, v3); }
inline void /*gl*/Uniform1fv(GLint location, GLsizei count, const GLfloat* value) { detail::glUniform1fv(location, count, value); }
inline void /*gl*/Uniform2fv(GLint location, GLsizei count, const GLfloat* value) { detail::glUniform2fv(location, count, value); }
inline void /*gl*/Uniform3fv(GLint location, GLsizei count, const GLfloat* value) { detail::glUniform3fv(location, count, value); }
inline void /*gl*/Uniform4fv(GLint location, GLsizei count, const GLfloat* value) { detail::glUniform4fv(location, count, value); }
inline void /*gl*/Uniform1iv(GLint location, GLsizei count, const GLint* value) { detail::glUniform1iv(location, count, value); }
inline void /*gl*/Uniform2iv(GLint location, GLsizei count, const GLint* value) { detail::glUniform2iv(location, count, value); }
inline void /*gl*/Uniform3iv(GLint location, GLsizei count, const GLint* value) { detail::glUniform3iv(location, count, value); }
inline void /*gl*/Uniform4iv(GLint location, GLsizei count, const GLint* value) { detail::glUniform4iv(location, count, value); }
inline void /*gl*/UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix2fv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix3fv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix4fv(location, count, transpose, value); }
inline void /*gl*/ValidateProgram(GLuint program) { detail::glValidateProgram(program); }
inline void /*gl*/VertexAttrib1d(GLuint index, GLdouble x) { detail::glVertexAttrib1d(index, x); }
inline void /*gl*/VertexAttrib1dv(GLuint index, const GLdouble* v) { detail::glVertexAttrib1dv(index, v); }
inline void /*gl*/VertexAttrib1f(GLuint index, GLfloat x) { detail::glVertexAttrib1f(index, x); }
inline void /*gl*/VertexAttrib1fv(GLuint index, const GLfloat* v) { detail::glVertexAttrib1fv(index, v); }
inline void /*gl*/VertexAttrib1s(GLuint index, GLshort x) { detail::glVertexAttrib1s(index, x); }
inline void /*gl*/VertexAttrib1sv(GLuint index, const GLshort* v) { detail::glVertexAttrib1sv(index, v); }
inline void /*gl*/VertexAttrib2d(GLuint index, GLdouble x, GLdouble y) { detail::glVertexAttrib2d(index, x, y); }
inline void /*gl*/VertexAttrib2dv(GLuint index, const GLdouble* v) { detail::glVertexAttrib2dv(index, v); }
inline void /*gl*/VertexAttrib2f(GLuint index, GLfloat x, GLfloat y) { detail::glVertexAttrib2f(index, x, y); }
inline void /*gl*/VertexAttrib2fv(GLuint index, const GLfloat* v) { detail::glVertexAttrib2fv(index, v); }
inline void /*gl*/VertexAttrib2s(GLuint index, GLshort x, GLshort y) { detail::glVertexAttrib2s(index, x, y); }
inline void /*gl*/VertexAttrib2sv(GLuint index, const GLshort* v) { detail::glVertexAttrib2sv(index, v); }
inline void /*gl*/VertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z) { detail::glVertexAttrib3d(index, x, y, z); }
inline void /*gl*/VertexAttrib3dv(GLuint index, const GLdouble* v) { detail::glVertexAttrib3dv(index, v); }
inline void /*gl*/VertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z) { detail::glVertexAttrib3f(index, x, y, z); }
inline void /*gl*/VertexAttrib3fv(GLuint index, const GLfloat* v) { detail::glVertexAttrib3fv(index, v); }
inline void /*gl*/VertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z) { detail::glVertexAttrib3s(index, x, y, z); }
inline void /*gl*/VertexAttrib3sv(GLuint index, const GLshort* v) { detail::glVertexAttrib3sv(index, v); }
inline void /*gl*/VertexAttrib4Nbv(GLuint index, const GLbyte* v) { detail::glVertexAttrib4Nbv(index, v); }
inline void /*gl*/VertexAttrib4Niv(GLuint index, const GLint* v) { detail::glVertexAttrib4Niv(index, v); }
inline void /*gl*/VertexAttrib4Nsv(GLuint index, const GLshort* v) { detail::glVertexAttrib4Nsv(index, v); }
inline void /*gl*/VertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w) { detail::glVertexAttrib4Nub(index, x, y, z, w); }
inline void /*gl*/VertexAttrib4Nubv(GLuint index, const GLubyte* v) { detail::glVertexAttrib4Nubv(index, v); }
inline void /*gl*/VertexAttrib4Nuiv(GLuint index, const GLuint* v) { detail::glVertexAttrib4Nuiv(index, v); }
inline void /*gl*/VertexAttrib4Nusv(GLuint index, const GLushort* v) { detail::glVertexAttrib4Nusv(index, v); }
inline void /*gl*/VertexAttrib4bv(GLuint index, const GLbyte* v) { detail::glVertexAttrib4bv(index, v); }
inline void /*gl*/VertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w) { detail::glVertexAttrib4d(index, x, y, z, w); }
inline void /*gl*/VertexAttrib4dv(GLuint index, const GLdouble* v) { detail::glVertexAttrib4dv(index, v); }
inline void /*gl*/VertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { detail::glVertexAttrib4f(index, x, y, z, w); }
inline void /*gl*/VertexAttrib4fv(GLuint index, const GLfloat* v) { detail::glVertexAttrib4fv(index, v); }
inline void /*gl*/VertexAttrib4iv(GLuint index, const GLint* v) { detail::glVertexAttrib4iv(index, v); }
inline void /*gl*/VertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w) { detail::glVertexAttrib4s(index, x, y, z, w); }
inline void /*gl*/VertexAttrib4sv(GLuint index, const GLshort* v) { detail::glVertexAttrib4sv(index, v); }
inline void /*gl*/VertexAttrib4ubv(GLuint index, const GLubyte* v) { detail::glVertexAttrib4ubv(index, v); }
inline void /*gl*/VertexAttrib4uiv(GLuint index, const GLuint* v) { detail::glVertexAttrib4uiv(index, v); }
inline void /*gl*/VertexAttrib4usv(GLuint index, const GLushort* v) { detail::glVertexAttrib4usv(index, v); }
inline void /*gl*/VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) { detail::glVertexAttribPointer(index, size, type, normalized, stride, pointer); }

// OpenGL 2.1
inline void /*gl*/UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix2x3fv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix3x2fv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix2x4fv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix4x2fv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix3x4fv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glUniformMatrix4x3fv(location, count, transpose, value); }

// OpenGL 3.0
inline void /*gl*/ColorMaski(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a) { detail::glColorMaski(index, r, g, b, a); }
inline void /*gl*/GetBooleani_v(GLenum target, GLuint index, GLboolean* data) { detail::glGetBooleani_v(target, index, data); }
inline void /*gl*/GetIntegeri_v(GLenum target, GLuint index, GLint* data) { detail::glGetIntegeri_v(target, index, data); }
inline void /*gl*/Enablei(GLenum target, GLuint index) { detail::glEnablei(target, index); }
inline void /*gl*/Disablei(GLenum target, GLuint index) { detail::glDisablei(target, index); }
inline GLboolean /*gl*/IsEnabledi(GLenum target, GLuint index) { return detail::glIsEnabledi(target, index); }
inline void /*gl*/BeginTransformFeedback(GLenum primitiveMode) { detail::glBeginTransformFeedback(primitiveMode); }
inline void /*gl*/EndTransformFeedback() { detail::glEndTransformFeedback(); }
inline void /*gl*/BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) { detail::glBindBufferRange(target, index, buffer, offset, size); }
inline void /*gl*/BindBufferBase(GLenum target, GLuint index, GLuint buffer) { detail::glBindBufferBase(target, index, buffer); }
inline void /*gl*/TransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode) { detail::glTransformFeedbackVaryings(program, count, varyings, bufferMode); }
inline void /*gl*/GetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name) { detail::glGetTransformFeedbackVarying(program, index, bufSize, length, size, type, name); }
inline void /*gl*/ClampColor(GLenum target, GLenum clamp) { detail::glClampColor(target, clamp); }
inline void /*gl*/BeginConditionalRender(GLuint id, GLenum mode) { detail::glBeginConditionalRender(id, mode); }
inline void /*gl*/EndConditionalRender() { detail::glEndConditionalRender(); }
inline void /*gl*/VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer) { detail::glVertexAttribIPointer(index, size, type, stride, pointer); }
inline void /*gl*/GetVertexAttribIiv(GLuint index, GLenum pname, GLint* params) { detail::glGetVertexAttribIiv(index, pname, params); }
inline void /*gl*/GetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params) { detail::glGetVertexAttribIuiv(index, pname, params); }
inline void /*gl*/VertexAttribI1i(GLuint index, GLint x) { detail::glVertexAttribI1i(index, x); }
inline void /*gl*/VertexAttribI2i(GLuint index, GLint x, GLint y) { detail::glVertexAttribI2i(index, x, y); }
inline void /*gl*/VertexAttribI3i(GLuint index, GLint x, GLint y, GLint z) { detail::glVertexAttribI3i(index, x, y, z); }
inline void /*gl*/VertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w) { detail::glVertexAttribI4i(index, x, y, z, w); }
inline void /*gl*/VertexAttribI1ui(GLuint index, GLuint x) { detail::glVertexAttribI1ui(index, x); }
inline void /*gl*/VertexAttribI2ui(GLuint index, GLuint x, GLuint y) { detail::glVertexAttribI2ui(index, x, y); }
inline void /*gl*/VertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z) { detail::glVertexAttribI3ui(index, x, y, z); }
inline void /*gl*/VertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) { detail::glVertexAttribI4ui(index, x, y, z, w); }
inline void /*gl*/VertexAttribI1iv(GLuint index, const GLint* v) { detail::glVertexAttribI1iv(index, v); }
inline void /*gl*/VertexAttribI2iv(GLuint index, const GLint* v) { detail::glVertexAttribI2iv(index, v); }
inline void /*gl*/VertexAttribI3iv(GLuint index, const GLint* v) { detail::glVertexAttribI3iv(index, v); }
inline void /*gl*/VertexAttribI4iv(GLuint index, const GLint* v) { detail::glVertexAttribI4iv(index, v); }
inline void /*gl*/VertexAttribI1uiv(GLuint index, const GLuint* v) { detail::glVertexAttribI1uiv(index, v); }
inline void /*gl*/VertexAttribI2uiv(GLuint index, const GLuint* v) { detail::glVertexAttribI2uiv(index, v); }
inline void /*gl*/VertexAttribI3uiv(GLuint index, const GLuint* v) { detail::glVertexAttribI3uiv(index, v); }
inline void /*gl*/VertexAttribI4uiv(GLuint index, const GLuint* v) { detail::glVertexAttribI4uiv(index, v); }
inline void /*gl*/VertexAttribI4bv(GLuint index, const GLbyte* v) { detail::glVertexAttribI4bv(index, v); }
inline void /*gl*/VertexAttribI4sv(GLuint index, const GLshort* v) { detail::glVertexAttribI4sv(index, v); }
inline void /*gl*/VertexAttribI4ubv(GLuint index, const GLubyte* v) { detail::glVertexAttribI4ubv(index, v); }
inline void /*gl*/VertexAttribI4usv(GLuint index, const GLushort* v) { detail::glVertexAttribI4usv(index, v); }
inline void /*gl*/GetUniformuiv(GLuint program, GLint location, GLuint* params) { detail::glGetUniformuiv(program, location, params); }
inline void /*gl*/BindFragDataLocation(GLuint program, GLuint color, const GLchar* name) { detail::glBindFragDataLocation(program, color, name); }
inline GLint /*gl*/GetFragDataLocation(GLuint program, const GLchar* name) { return detail::glGetFragDataLocation(program, name); }
inline void /*gl*/Uniform1ui(GLint location, GLuint v0) { detail::glUniform1ui(location, v0); }
inline void /*gl*/Uniform2ui(GLint location, GLuint v0, GLuint v1) { detail::glUniform2ui(location, v0, v1); }
inline void /*gl*/Uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2) { detail::glUniform3ui(location, v0, v1, v2); }
inline void /*gl*/Uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) { detail::glUniform4ui(location, v0, v1, v2, v3); }
inline void /*gl*/Uniform1uiv(GLint location, GLsizei count, const GLuint* value) { detail::glUniform1uiv(location, count, value); }
inline void /*gl*/Uniform2uiv(GLint location, GLsizei count, const GLuint* value) { detail::glUniform2uiv(location, count, value); }
inline void /*gl*/Uniform3uiv(GLint location, GLsizei count, const GLuint* value) { detail::glUniform3uiv(location, count, value); }
inline void /*gl*/Uniform4uiv(GLint location, GLsizei count, const GLuint* value) { detail::glUniform4uiv(location, count, value); }
inline void /*gl*/TexParameterIiv(GLenum target, GLenum pname, const GLint* params) { detail::glTexParameterIiv(target, pname, params); }
inline void /*gl*/TexParameterIuiv(GLenum target, GLenum pname, const GLuint* params) { detail::glTexParameterIuiv(target, pname, params); }
inline void /*gl*/GetTexParameterIiv(GLenum target, GLenum pname, GLint* params) { detail::glGetTexParameterIiv(target, pname, params); }
inline void /*gl*/GetTexParameterIuiv(GLenum target, GLenum pname, GLuint* params) { detail::glGetTexParameterIuiv(target, pname, params); }
inline void /*gl*/ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value) { detail::glClearBufferiv(buffer, drawbuffer, value); }
inline void /*gl*/ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value) { detail::glClearBufferuiv(buffer, drawbuffer, value); }
inline void /*gl*/ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value) { detail::glClearBufferfv(buffer, drawbuffer, value); }
inline void /*gl*/ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) { detail::glClearBufferfi(buffer, drawbuffer, depth, stencil); }
inline const GLubyte* /*gl*/GetStringi(GLenum name, GLuint index) { return detail::glGetStringi(name, index); }
inline GLboolean /*gl*/IsRenderbuffer(GLuint renderbuffer) { return detail::glIsRenderbuffer(renderbuffer); }
inline void /*gl*/BindRenderbuffer(GLenum target, GLuint renderbuffer) { detail::glBindRenderbuffer(target, renderbuffer); }
inline void /*gl*/DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers) { detail::glDeleteRenderbuffers(n, renderbuffers); }
inline void /*gl*/GenRenderbuffers(GLsizei n, GLuint* renderbuffers) { detail::glGenRenderbuffers(n, renderbuffers); }
inline void /*gl*/RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) { detail::glRenderbufferStorage(target, internalformat, width, height); }
inline void /*gl*/GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params) { detail::glGetRenderbufferParameteriv(target, pname, params); }
inline GLboolean /*gl*/IsFramebuffer(GLuint framebuffer) { return detail::glIsFramebuffer(framebuffer); }
inline void /*gl*/BindFramebuffer(GLenum target, GLuint framebuffer) { detail::glBindFramebuffer(target, framebuffer); }
inline void /*gl*/DeleteFramebuffers(GLsizei n, const GLuint* framebuffers) { detail::glDeleteFramebuffers(n, framebuffers); }
inline void /*gl*/GenFramebuffers(GLsizei n, GLuint* framebuffers) { detail::glGenFramebuffers(n, framebuffers); }
inline GLenum /*gl*/CheckFramebufferStatus(GLenum target) { return detail::glCheckFramebufferStatus(target); }
inline void /*gl*/FramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) { detail::glFramebufferTexture1D(target, attachment, textarget, texture, level); }
inline void /*gl*/FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) { detail::glFramebufferTexture2D(target, attachment, textarget, texture, level); }
inline void /*gl*/FramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset) { detail::glFramebufferTexture3D(target, attachment, textarget, texture, level, zoffset); }
inline void /*gl*/FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) { detail::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer); }
inline void /*gl*/GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params) { detail::glGetFramebufferAttachmentParameteriv(target, attachment, pname, params); }
inline void /*gl*/GenerateMipmap(GLenum target) { detail::glGenerateMipmap(target); }
inline void /*gl*/BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) { detail::glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter); }
inline void /*gl*/RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) { detail::glRenderbufferStorageMultisample(target, samples, internalformat, width, height); }
inline void /*gl*/FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) { detail::glFramebufferTextureLayer(target, attachment, texture, level, layer); }
inline void* /*gl*/MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) { return detail::glMapBufferRange(target, offset, length, access); }
inline void /*gl*/FlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length) { detail::glFlushMappedBufferRange(target, offset, length); }
inline void /*gl*/BindVertexArray(GLuint array) { detail::glBindVertexArray(array); }
inline void /*gl*/DeleteVertexArrays(GLsizei n, const GLuint* arrays) { detail::glDeleteVertexArrays(n, arrays); }
inline void /*gl*/GenVertexArrays(GLsizei n, GLuint* arrays) { detail::glGenVertexArrays(n, arrays); }
inline GLboolean /*gl*/IsVertexArray(GLuint array) { return detail::glIsVertexArray(array); }

// OpenGL 3.1
inline void /*gl*/DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) { detail::glDrawArraysInstanced(mode, first, count, instancecount); }
inline void /*gl*/DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount) { detail::glDrawElementsInstanced(mode, count, type, indices, instancecount); }
inline void /*gl*/TexBuffer(GLenum target, GLenum internalformat, GLuint buffer) { detail::glTexBuffer(target, internalformat, buffer); }
inline void /*gl*/PrimitiveRestartIndex(GLuint index) { detail::glPrimitiveRestartIndex(index); }
inline void /*gl*/CopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) { detail::glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size); }
inline void /*gl*/GetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices) { detail::glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices); }
inline void /*gl*/GetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params) { detail::glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params); }
inline void /*gl*/GetActiveUniformName(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformName) { detail::glGetActiveUniformName(program, uniformIndex, bufSize, length, uniformName); }
inline GLuint /*gl*/GetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName) { return detail::glGetUniformBlockIndex(program, uniformBlockName); }
inline void /*gl*/GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params) { detail::glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params); }
inline void /*gl*/GetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName) { detail::glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName); }
inline void /*gl*/UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) { detail::glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding); }

// OpenGL 3.2
inline void /*gl*/DrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex) { detail::glDrawElementsBaseVertex(mode, count, type, indices, basevertex); }
inline void /*gl*/DrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices, GLint basevertex) { detail::glDrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex); }
inline void /*gl*/DrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex) { detail::glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex); }
inline void /*gl*/MultiDrawElementsBaseVertex(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount, const GLint* basevertex) { detail::glMultiDrawElementsBaseVertex(mode, count, type, indices, drawcount, basevertex); }
inline void /*gl*/ProvokingVertex(GLenum mode) { detail::glProvokingVertex(mode); }
inline GLsync /*gl*/FenceSync(GLenum condition, GLbitfield flags) { return detail::glFenceSync(condition, flags); }
inline GLboolean /*gl*/IsSync(GLsync sync) { return detail::glIsSync(sync); }
inline void /*gl*/DeleteSync(GLsync sync) { detail::glDeleteSync(sync); }
inline GLenum /*gl*/ClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) { return detail::glClientWaitSync(sync, flags, timeout); }
inline void /*gl*/WaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) { detail::glWaitSync(sync, flags, timeout); }
inline void /*gl*/GetInteger64v(GLenum pname, GLint64* data) { detail::glGetInteger64v(pname, data); }
inline void /*gl*/GetSynciv(GLsync sync, GLenum pname, GLsizei count, GLsizei* length, GLint* values) { detail::glGetSynciv(sync, pname, count, length, values); }
inline void /*gl*/GetInteger64i_v(GLenum target, GLuint index, GLint64* data) { detail::glGetInteger64i_v(target, index, data); }
inline void /*gl*/GetBufferParameteri64v(GLenum target, GLenum pname, GLint64* params) { detail::glGetBufferParameteri64v(target, pname, params); }
inline void /*gl*/FramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level) { detail::glFramebufferTexture(target, attachment, texture, level); }
inline void /*gl*/TexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) { detail::glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations); }
inline void /*gl*/TexImage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) { detail::glTexImage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations); }
inline void /*gl*/GetMultisamplefv(GLenum pname, GLuint index, GLfloat* val) { detail::glGetMultisamplefv(pname, index, val); }
inline void /*gl*/SampleMaski(GLuint maskNumber, GLbitfield mask) { detail::glSampleMaski(maskNumber, mask); }

// OpenGL 3.3
inline void /*gl*/BindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const GLchar* name) { detail::glBindFragDataLocationIndexed(program, colorNumber, index, name); }
inline GLint /*gl*/GetFragDataIndex(GLuint program, const GLchar* name) { return detail::glGetFragDataIndex(program, name); }
inline void /*gl*/GenSamplers(GLsizei count, GLuint* samplers) { detail::glGenSamplers(count, samplers); }
inline void /*gl*/DeleteSamplers(GLsizei count, const GLuint* samplers) { detail::glDeleteSamplers(count, samplers); }
inline GLboolean /*gl*/IsSampler(GLuint sampler) { return detail::glIsSampler(sampler); }
inline void /*gl*/BindSampler(GLuint unit, GLuint sampler) { detail::glBindSampler(unit, sampler); }
inline void /*gl*/SamplerParameteri(GLuint sampler, GLenum pname, GLint param) { detail::glSamplerParameteri(sampler, pname, param); }
inline void /*gl*/SamplerParameteriv(GLuint sampler, GLenum pname, const GLint* param) { detail::glSamplerParameteriv(sampler, pname, param); }
inline void /*gl*/SamplerParameterf(GLuint sampler, GLenum pname, GLfloat param) { detail::glSamplerParameterf(sampler, pname, param); }
inline void /*gl*/SamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat* param) { detail::glSamplerParameterfv(sampler, pname, param); }
inline void /*gl*/SamplerParameterIiv(GLuint sampler, GLenum pname, const GLint* param) { detail::glSamplerParameterIiv(sampler, pname, param); }
inline void /*gl*/SamplerParameterIuiv(GLuint sampler, GLenum pname, const GLuint* param) { detail::glSamplerParameterIuiv(sampler, pname, param); }
inline void /*gl*/GetSamplerParameteriv(GLuint sampler, GLenum pname, GLint* params) { detail::glGetSamplerParameteriv(sampler, pname, params); }
inline void /*gl*/GetSamplerParameterIiv(GLuint sampler, GLenum pname, GLint* params) { detail::glGetSamplerParameterIiv(sampler, pname, params); }
inline void /*gl*/GetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat* params) { detail::glGetSamplerParameterfv(sampler, pname, params); }
inline void /*gl*/GetSamplerParameterIuiv(GLuint sampler, GLenum pname, GLuint* params) { detail::glGetSamplerParameterIuiv(sampler, pname, params); }
inline void /*gl*/QueryCounter(GLuint id, GLenum target) { detail::glQueryCounter(id, target); }
inline void /*gl*/GetQueryObjecti64v(GLuint id, GLenum pname, GLint64* params) { detail::glGetQueryObjecti64v(id, pname, params); }
inline void /*gl*/GetQueryObjectui64v(GLuint id, GLenum pname, GLuint64* params) { detail::glGetQueryObjectui64v(id, pname, params); }
inline void /*gl*/VertexAttribDivisor(GLuint index, GLuint divisor) { detail::glVertexAttribDivisor(index, divisor); }
inline void /*gl*/VertexAttribP1ui(GLuint index, GLenum type, GLboolean normalized, GLuint value) { detail::glVertexAttribP1ui(index, type, normalized, value); }
inline void /*gl*/VertexAttribP1uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint* value) { detail::glVertexAttribP1uiv(index, type, normalized, value); }
inline void /*gl*/VertexAttribP2ui(GLuint index, GLenum type, GLboolean normalized, GLuint value) { detail::glVertexAttribP2ui(index, type, normalized, value); }
inline void /*gl*/VertexAttribP2uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint* value) { detail::glVertexAttribP2uiv(index, type, normalized, value); }
inline void /*gl*/VertexAttribP3ui(GLuint index, GLenum type, GLboolean normalized, GLuint value) { detail::glVertexAttribP3ui(index, type, normalized, value); }
inline void /*gl*/VertexAttribP3uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint* value) { detail::glVertexAttribP3uiv(index, type, normalized, value); }
inline void /*gl*/VertexAttribP4ui(GLuint index, GLenum type, GLboolean normalized, GLuint value) { detail::glVertexAttribP4ui(index, type, normalized, value); }
inline void /*gl*/VertexAttribP4uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint* value) { detail::glVertexAttribP4uiv(index, type, normalized, value); }
inline void /*gl*/VertexP2ui(GLenum type, GLuint value) { detail::glVertexP2ui(type, value); }
inline void /*gl*/VertexP2uiv(GLenum type, const GLuint* value) { detail::glVertexP2uiv(type, value); }
inline void /*gl*/VertexP3ui(GLenum type, GLuint value) { detail::glVertexP3ui(type, value); }
inline void /*gl*/VertexP3uiv(GLenum type, const GLuint* value) { detail::glVertexP3uiv(type, value); }
inline void /*gl*/VertexP4ui(GLenum type, GLuint value) { detail::glVertexP4ui(type, value); }
inline void /*gl*/VertexP4uiv(GLenum type, const GLuint* value) { detail::glVertexP4uiv(type, value); }
inline void /*gl*/TexCoordP1ui(GLenum type, GLuint coords) { detail::glTexCoordP1ui(type, coords); }
inline void /*gl*/TexCoordP1uiv(GLenum type, const GLuint* coords) { detail::glTexCoordP1uiv(type, coords); }
inline void /*gl*/TexCoordP2ui(GLenum type, GLuint coords) { detail::glTexCoordP2ui(type, coords); }
inline void /*gl*/TexCoordP2uiv(GLenum type, const GLuint* coords) { detail::glTexCoordP2uiv(type, coords); }
inline void /*gl*/TexCoordP3ui(GLenum type, GLuint coords) { detail::glTexCoordP3ui(type, coords); }
inline void /*gl*/TexCoordP3uiv(GLenum type, const GLuint* coords) { detail::glTexCoordP3uiv(type, coords); }
inline void /*gl*/TexCoordP4ui(GLenum type, GLuint coords) { detail::glTexCoordP4ui(type, coords); }
inline void /*gl*/TexCoordP4uiv(GLenum type, const GLuint* coords) { detail::glTexCoordP4uiv(type, coords); }
inline void /*gl*/MultiTexCoordP1ui(GLenum texture, GLenum type, GLuint coords) { detail::glMultiTexCoordP1ui(texture, type, coords); }
inline void /*gl*/MultiTexCoordP1uiv(GLenum texture, GLenum type, const GLuint* coords) { detail::glMultiTexCoordP1uiv(texture, type, coords); }
inline void /*gl*/MultiTexCoordP2ui(GLenum texture, GLenum type, GLuint coords) { detail::glMultiTexCoordP2ui(texture, type, coords); }
inline void /*gl*/MultiTexCoordP2uiv(GLenum texture, GLenum type, const GLuint* coords) { detail::glMultiTexCoordP2uiv(texture, type, coords); }
inline void /*gl*/MultiTexCoordP3ui(GLenum texture, GLenum type, GLuint coords) { detail::glMultiTexCoordP3ui(texture, type, coords); }
inline void /*gl*/MultiTexCoordP3uiv(GLenum texture, GLenum type, const GLuint* coords) { detail::glMultiTexCoordP3uiv(texture, type, coords); }
inline void /*gl*/MultiTexCoordP4ui(GLenum texture, GLenum type, GLuint coords) { detail::glMultiTexCoordP4ui(texture, type, coords); }
inline void /*gl*/MultiTexCoordP4uiv(GLenum texture, GLenum type, const GLuint* coords) { detail::glMultiTexCoordP4uiv(texture, type, coords); }
inline void /*gl*/NormalP3ui(GLenum type, GLuint coords) { detail::glNormalP3ui(type, coords); }
inline void /*gl*/NormalP3uiv(GLenum type, const GLuint* coords) { detail::glNormalP3uiv(type, coords); }
inline void /*gl*/ColorP3ui(GLenum type, GLuint color) { detail::glColorP3ui(type, color); }
inline void /*gl*/ColorP3uiv(GLenum type, const GLuint* color) { detail::glColorP3uiv(type, color); }
inline void /*gl*/ColorP4ui(GLenum type, GLuint color) { detail::glColorP4ui(type, color); }
inline void /*gl*/ColorP4uiv(GLenum type, const GLuint* color) { detail::glColorP4uiv(type, color); }
inline void /*gl*/SecondaryColorP3ui(GLenum type, GLuint color) { detail::glSecondaryColorP3ui(type, color); }
inline void /*gl*/SecondaryColorP3uiv(GLenum type, const GLuint* color) { detail::glSecondaryColorP3uiv(type, color); }

// OpenGL 4.0
inline void /*gl*/MinSampleShading(GLfloat value) { detail::glMinSampleShading(value); }
inline void /*gl*/BlendEquationi(GLuint buf, GLenum mode) { detail::glBlendEquationi(buf, mode); }
inline void /*gl*/BlendEquationSeparatei(GLuint buf, GLenum modeRGB, GLenum modeAlpha) { detail::glBlendEquationSeparatei(buf, modeRGB, modeAlpha); }
inline void /*gl*/BlendFunci(GLuint buf, GLenum src, GLenum dst) { detail::glBlendFunci(buf, src, dst); }
inline void /*gl*/BlendFuncSeparatei(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) { detail::glBlendFuncSeparatei(buf, srcRGB, dstRGB, srcAlpha, dstAlpha); }
inline void /*gl*/DrawArraysIndirect(GLenum mode, const void* indirect) { detail::glDrawArraysIndirect(mode, indirect); }
inline void /*gl*/DrawElementsIndirect(GLenum mode, GLenum type, const void* indirect) { detail::glDrawElementsIndirect(mode, type, indirect); }
inline void /*gl*/Uniform1d(GLint location, GLdouble x) { detail::glUniform1d(location, x); }
inline void /*gl*/Uniform2d(GLint location, GLdouble x, GLdouble y) { detail::glUniform2d(location, x, y); }
inline void /*gl*/Uniform3d(GLint location, GLdouble x, GLdouble y, GLdouble z) { detail::glUniform3d(location, x, y, z); }
inline void /*gl*/Uniform4d(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w) { detail::glUniform4d(location, x, y, z, w); }
inline void /*gl*/Uniform1dv(GLint location, GLsizei count, const GLdouble* value) { detail::glUniform1dv(location, count, value); }
inline void /*gl*/Uniform2dv(GLint location, GLsizei count, const GLdouble* value) { detail::glUniform2dv(location, count, value); }
inline void /*gl*/Uniform3dv(GLint location, GLsizei count, const GLdouble* value) { detail::glUniform3dv(location, count, value); }
inline void /*gl*/Uniform4dv(GLint location, GLsizei count, const GLdouble* value) { detail::glUniform4dv(location, count, value); }
inline void /*gl*/UniformMatrix2dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix2dv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix3dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix3dv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix4dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix4dv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix2x3dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix2x3dv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix2x4dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix2x4dv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix3x2dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix3x2dv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix3x4dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix3x4dv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix4x2dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix4x2dv(location, count, transpose, value); }
inline void /*gl*/UniformMatrix4x3dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glUniformMatrix4x3dv(location, count, transpose, value); }
inline void /*gl*/GetUniformdv(GLuint program, GLint location, GLdouble* params) { detail::glGetUniformdv(program, location, params); }
inline GLint /*gl*/GetSubroutineUniformLocation(GLuint program, GLenum shadertype, const GLchar* name) { return detail::glGetSubroutineUniformLocation(program, shadertype, name); }
inline GLuint /*gl*/GetSubroutineIndex(GLuint program, GLenum shadertype, const GLchar* name) { return detail::glGetSubroutineIndex(program, shadertype, name); }
inline void /*gl*/GetActiveSubroutineUniformiv(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint* values) { detail::glGetActiveSubroutineUniformiv(program, shadertype, index, pname, values); }
inline void /*gl*/GetActiveSubroutineUniformName(GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name) { detail::glGetActiveSubroutineUniformName(program, shadertype, index, bufSize, length, name); }
inline void /*gl*/GetActiveSubroutineName(GLuint program, GLenum shadertype, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name) { detail::glGetActiveSubroutineName(program, shadertype, index, bufSize, length, name); }
inline void /*gl*/UniformSubroutinesuiv(GLenum shadertype, GLsizei count, const GLuint* indices) { detail::glUniformSubroutinesuiv(shadertype, count, indices); }
inline void /*gl*/GetUniformSubroutineuiv(GLenum shadertype, GLint location, GLuint* params) { detail::glGetUniformSubroutineuiv(shadertype, location, params); }
inline void /*gl*/GetProgramStageiv(GLuint program, GLenum shadertype, GLenum pname, GLint* values) { detail::glGetProgramStageiv(program, shadertype, pname, values); }
inline void /*gl*/PatchParameteri(GLenum pname, GLint value) { detail::glPatchParameteri(pname, value); }
inline void /*gl*/PatchParameterfv(GLenum pname, const GLfloat* values) { detail::glPatchParameterfv(pname, values); }
inline void /*gl*/BindTransformFeedback(GLenum target, GLuint id) { detail::glBindTransformFeedback(target, id); }
inline void /*gl*/DeleteTransformFeedbacks(GLsizei n, const GLuint* ids) { detail::glDeleteTransformFeedbacks(n, ids); }
inline void /*gl*/GenTransformFeedbacks(GLsizei n, GLuint* ids) { detail::glGenTransformFeedbacks(n, ids); }
inline GLboolean /*gl*/IsTransformFeedback(GLuint id) { return detail::glIsTransformFeedback(id); }
inline void /*gl*/PauseTransformFeedback() { detail::glPauseTransformFeedback(); }
inline void /*gl*/ResumeTransformFeedback() { detail::glResumeTransformFeedback(); }
inline void /*gl*/DrawTransformFeedback(GLenum mode, GLuint id) { detail::glDrawTransformFeedback(mode, id); }
inline void /*gl*/DrawTransformFeedbackStream(GLenum mode, GLuint id, GLuint stream) { detail::glDrawTransformFeedbackStream(mode, id, stream); }
inline void /*gl*/BeginQueryIndexed(GLenum target, GLuint index, GLuint id) { detail::glBeginQueryIndexed(target, index, id); }
inline void /*gl*/EndQueryIndexed(GLenum target, GLuint index) { detail::glEndQueryIndexed(target, index); }
inline void /*gl*/GetQueryIndexediv(GLenum target, GLuint index, GLenum pname, GLint* params) { detail::glGetQueryIndexediv(target, index, pname, params); }

// OpenGL 4.1
inline void /*gl*/ReleaseShaderCompiler() { detail::glReleaseShaderCompiler(); }
inline void /*gl*/ShaderBinary(GLsizei count, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length) { detail::glShaderBinary(count, shaders, binaryformat, binary, length); }
inline void /*gl*/GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) { detail::glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision); }
inline void /*gl*/DepthRangef(GLfloat n, GLfloat f) { detail::glDepthRangef(n, f); }
inline void /*gl*/ClearDepthf(GLfloat d) { detail::glClearDepthf(d); }
inline void /*gl*/GetProgramBinary(GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, void* binary) { detail::glGetProgramBinary(program, bufSize, length, binaryFormat, binary); }
inline void /*gl*/ProgramBinary(GLuint program, GLenum binaryFormat, const void* binary, GLsizei length) { detail::glProgramBinary(program, binaryFormat, binary, length); }
inline void /*gl*/ProgramParameteri(GLuint program, GLenum pname, GLint value) { detail::glProgramParameteri(program, pname, value); }
inline void /*gl*/UseProgramStages(GLuint pipeline, GLbitfield stages, GLuint program) { detail::glUseProgramStages(pipeline, stages, program); }
inline void /*gl*/ActiveShaderProgram(GLuint pipeline, GLuint program) { detail::glActiveShaderProgram(pipeline, program); }
inline GLuint /*gl*/CreateShaderProgramv(GLenum type, GLsizei count, const GLchar* const* strings) { return detail::glCreateShaderProgramv(type, count, strings); }
inline void /*gl*/BindProgramPipeline(GLuint pipeline) { detail::glBindProgramPipeline(pipeline); }
inline void /*gl*/DeleteProgramPipelines(GLsizei n, const GLuint* pipelines) { detail::glDeleteProgramPipelines(n, pipelines); }
inline void /*gl*/GenProgramPipelines(GLsizei n, GLuint* pipelines) { detail::glGenProgramPipelines(n, pipelines); }
inline GLboolean /*gl*/IsProgramPipeline(GLuint pipeline) { return detail::glIsProgramPipeline(pipeline); }
inline void /*gl*/GetProgramPipelineiv(GLuint pipeline, GLenum pname, GLint* params) { detail::glGetProgramPipelineiv(pipeline, pname, params); }
inline void /*gl*/ProgramUniform1i(GLuint program, GLint location, GLint v0) { detail::glProgramUniform1i(program, location, v0); }
inline void /*gl*/ProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint* value) { detail::glProgramUniform1iv(program, location, count, value); }
inline void /*gl*/ProgramUniform1f(GLuint program, GLint location, GLfloat v0) { detail::glProgramUniform1f(program, location, v0); }
inline void /*gl*/ProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat* value) { detail::glProgramUniform1fv(program, location, count, value); }
inline void /*gl*/ProgramUniform1d(GLuint program, GLint location, GLdouble v0) { detail::glProgramUniform1d(program, location, v0); }
inline void /*gl*/ProgramUniform1dv(GLuint program, GLint location, GLsizei count, const GLdouble* value) { detail::glProgramUniform1dv(program, location, count, value); }
inline void /*gl*/ProgramUniform1ui(GLuint program, GLint location, GLuint v0) { detail::glProgramUniform1ui(program, location, v0); }
inline void /*gl*/ProgramUniform1uiv(GLuint program, GLint location, GLsizei count, const GLuint* value) { detail::glProgramUniform1uiv(program, location, count, value); }
inline void /*gl*/ProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1) { detail::glProgramUniform2i(program, location, v0, v1); }
inline void /*gl*/ProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint* value) { detail::glProgramUniform2iv(program, location, count, value); }
inline void /*gl*/ProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1) { detail::glProgramUniform2f(program, location, v0, v1); }
inline void /*gl*/ProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat* value) { detail::glProgramUniform2fv(program, location, count, value); }
inline void /*gl*/ProgramUniform2d(GLuint program, GLint location, GLdouble v0, GLdouble v1) { detail::glProgramUniform2d(program, location, v0, v1); }
inline void /*gl*/ProgramUniform2dv(GLuint program, GLint location, GLsizei count, const GLdouble* value) { detail::glProgramUniform2dv(program, location, count, value); }
inline void /*gl*/ProgramUniform2ui(GLuint program, GLint location, GLuint v0, GLuint v1) { detail::glProgramUniform2ui(program, location, v0, v1); }
inline void /*gl*/ProgramUniform2uiv(GLuint program, GLint location, GLsizei count, const GLuint* value) { detail::glProgramUniform2uiv(program, location, count, value); }
inline void /*gl*/ProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2) { detail::glProgramUniform3i(program, location, v0, v1, v2); }
inline void /*gl*/ProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint* value) { detail::glProgramUniform3iv(program, location, count, value); }
inline void /*gl*/ProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2) { detail::glProgramUniform3f(program, location, v0, v1, v2); }
inline void /*gl*/ProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat* value) { detail::glProgramUniform3fv(program, location, count, value); }
inline void /*gl*/ProgramUniform3d(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2) { detail::glProgramUniform3d(program, location, v0, v1, v2); }
inline void /*gl*/ProgramUniform3dv(GLuint program, GLint location, GLsizei count, const GLdouble* value) { detail::glProgramUniform3dv(program, location, count, value); }
inline void /*gl*/ProgramUniform3ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2) { detail::glProgramUniform3ui(program, location, v0, v1, v2); }
inline void /*gl*/ProgramUniform3uiv(GLuint program, GLint location, GLsizei count, const GLuint* value) { detail::glProgramUniform3uiv(program, location, count, value); }
inline void /*gl*/ProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3) { detail::glProgramUniform4i(program, location, v0, v1, v2, v3); }
inline void /*gl*/ProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint* value) { detail::glProgramUniform4iv(program, location, count, value); }
inline void /*gl*/ProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { detail::glProgramUniform4f(program, location, v0, v1, v2, v3); }
inline void /*gl*/ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value) { detail::glProgramUniform4fv(program, location, count, value); }
inline void /*gl*/ProgramUniform4d(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3) { detail::glProgramUniform4d(program, location, v0, v1, v2, v3); }
inline void /*gl*/ProgramUniform4dv(GLuint program, GLint location, GLsizei count, const GLdouble* value) { detail::glProgramUniform4dv(program, location, count, value); }
inline void /*gl*/ProgramUniform4ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) { detail::glProgramUniform4ui(program, location, v0, v1, v2, v3); }
inline void /*gl*/ProgramUniform4uiv(GLuint program, GLint location, GLsizei count, const GLuint* value) { detail::glProgramUniform4uiv(program, location, count, value); }
inline void /*gl*/ProgramUniformMatrix2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix2fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix3fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix4fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix2dv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix3dv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix4dv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix2x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix2x3fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix3x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix3x2fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix2x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix2x4fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix4x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix4x2fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix3x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix3x4fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix4x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) { detail::glProgramUniformMatrix4x3fv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix2x3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix2x3dv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix3x2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix3x2dv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix2x4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix2x4dv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix4x2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix4x2dv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix3x4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix3x4dv(program, location, count, transpose, value); }
inline void /*gl*/ProgramUniformMatrix4x3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) { detail::glProgramUniformMatrix4x3dv(program, location, count, transpose, value); }
inline void /*gl*/ValidateProgramPipeline(GLuint pipeline) { detail::glValidateProgramPipeline(pipeline); }
inline void /*gl*/GetProgramPipelineInfoLog(GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog) { detail::glGetProgramPipelineInfoLog(pipeline, bufSize, length, infoLog); }
inline void /*gl*/VertexAttribL1d(GLuint index, GLdouble x) { detail::glVertexAttribL1d(index, x); }
inline void /*gl*/VertexAttribL2d(GLuint index, GLdouble x, GLdouble y) { detail::glVertexAttribL2d(index, x, y); }
inline void /*gl*/VertexAttribL3d(GLuint index, GLdouble x, GLdouble y, GLdouble z) { detail::glVertexAttribL3d(index, x, y, z); }
inline void /*gl*/VertexAttribL4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w) { detail::glVertexAttribL4d(index, x, y, z, w); }
inline void /*gl*/VertexAttribL1dv(GLuint index, const GLdouble* v) { detail::glVertexAttribL1dv(index, v); }
inline void /*gl*/VertexAttribL2dv(GLuint index, const GLdouble* v) { detail::glVertexAttribL2dv(index, v); }
inline void /*gl*/VertexAttribL3dv(GLuint index, const GLdouble* v) { detail::glVertexAttribL3dv(index, v); }
inline void /*gl*/VertexAttribL4dv(GLuint index, const GLdouble* v) { detail::glVertexAttribL4dv(index, v); }
inline void /*gl*/VertexAttribLPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer) { detail::glVertexAttribLPointer(index, size, type, stride, pointer); }
inline void /*gl*/GetVertexAttribLdv(GLuint index, GLenum pname, GLdouble* params) { detail::glGetVertexAttribLdv(index, pname, params); }
inline void /*gl*/ViewportArrayv(GLuint first, GLsizei count, const GLfloat* v) { detail::glViewportArrayv(first, count, v); }
inline void /*gl*/ViewportIndexedf(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h) { detail::glViewportIndexedf(index, x, y, w, h); }
inline void /*gl*/ViewportIndexedfv(GLuint index, const GLfloat* v) { detail::glViewportIndexedfv(index, v); }
inline void /*gl*/ScissorArrayv(GLuint first, GLsizei count, const GLint* v) { detail::glScissorArrayv(first, count, v); }
inline void /*gl*/ScissorIndexed(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height) { detail::glScissorIndexed(index, left, bottom, width, height); }
inline void /*gl*/ScissorIndexedv(GLuint index, const GLint* v) { detail::glScissorIndexedv(index, v); }
inline void /*gl*/DepthRangeArrayv(GLuint first, GLsizei count, const GLdouble* v) { detail::glDepthRangeArrayv(first, count, v); }
inline void /*gl*/DepthRangeIndexed(GLuint index, GLdouble n, GLdouble f) { detail::glDepthRangeIndexed(index, n, f); }
inline void /*gl*/GetFloati_v(GLenum target, GLuint index, GLfloat* data) { detail::glGetFloati_v(target, index, data); }
inline void /*gl*/GetDoublei_v(GLenum target, GLuint index, GLdouble* data) { detail::glGetDoublei_v(target, index, data); }

// OpenGL 4.2
inline void /*gl*/DrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance) { detail::glDrawArraysInstancedBaseInstance(mode, first, count, instancecount, baseinstance); }
inline void /*gl*/DrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance) { detail::glDrawElementsInstancedBaseInstance(mode, count, type, indices, instancecount, baseinstance); }
inline void /*gl*/DrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance) { detail::glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, indices, instancecount, basevertex, baseinstance); }
inline void /*gl*/GetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint* params) { detail::glGetInternalformativ(target, internalformat, pname, count, params); }
inline void /*gl*/GetActiveAtomicCounterBufferiv(GLuint program, GLuint bufferIndex, GLenum pname, GLint* params) { detail::glGetActiveAtomicCounterBufferiv(program, bufferIndex, pname, params); }
inline void /*gl*/BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) { detail::glBindImageTexture(unit, texture, level, layered, layer, access, format); }
inline void /*gl*/MemoryBarrier(GLbitfield barriers) { detail::glMemoryBarrier(barriers); }
inline void /*gl*/TexStorage1D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width) { detail::glTexStorage1D(target, levels, internalformat, width); }
inline void /*gl*/TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) { detail::glTexStorage2D(target, levels, internalformat, width, height); }
inline void /*gl*/TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) { detail::glTexStorage3D(target, levels, internalformat, width, height, depth); }
inline void /*gl*/DrawTransformFeedbackInstanced(GLenum mode, GLuint id, GLsizei instancecount) { detail::glDrawTransformFeedbackInstanced(mode, id, instancecount); }
inline void /*gl*/DrawTransformFeedbackStreamInstanced(GLenum mode, GLuint id, GLuint stream, GLsizei instancecount) { detail::glDrawTransformFeedbackStreamInstanced(mode, id, stream, instancecount); }

// OpenGL 4.3
inline void /*gl*/ClearBufferData(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void* data) { detail::glClearBufferData(target, internalformat, format, type, data); }
inline void /*gl*/ClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data) { detail::glClearBufferSubData(target, internalformat, offset, size, format, type, data); }
inline void /*gl*/DispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) { detail::glDispatchCompute(num_groups_x, num_groups_y, num_groups_z); }
inline void /*gl*/DispatchComputeIndirect(GLintptr indirect) { detail::glDispatchComputeIndirect(indirect); }
inline void /*gl*/CopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth) { detail::glCopyImageSubData(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth); }
inline void /*gl*/FramebufferParameteri(GLenum target, GLenum pname, GLint param) { detail::glFramebufferParameteri(target, pname, param); }
inline void /*gl*/GetFramebufferParameteriv(GLenum target, GLenum pname, GLint* params) { detail::glGetFramebufferParameteriv(target, pname, params); }
inline void /*gl*/GetInternalformati64v(GLenum target, GLenum internalformat, GLenum pname, GLsizei count, GLint64* params) { detail::glGetInternalformati64v(target, internalformat, pname, count, params); }
inline void /*gl*/InvalidateTexSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth) { detail::glInvalidateTexSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth); }
inline void /*gl*/InvalidateTexImage(GLuint texture, GLint level) { detail::glInvalidateTexImage(texture, level); }
inline void /*gl*/InvalidateBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr length) { detail::glInvalidateBufferSubData(buffer, offset, length); }
inline void /*gl*/InvalidateBufferData(GLuint buffer) { detail::glInvalidateBufferData(buffer); }
inline void /*gl*/InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments) { detail::glInvalidateFramebuffer(target, numAttachments, attachments); }
inline void /*gl*/InvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height) { detail::glInvalidateSubFramebuffer(target, numAttachments, attachments, x, y, width, height); }
inline void /*gl*/MultiDrawArraysIndirect(GLenum mode, const void* indirect, GLsizei drawcount, GLsizei stride) { detail::glMultiDrawArraysIndirect(mode, indirect, drawcount, stride); }
inline void /*gl*/MultiDrawElementsIndirect(GLenum mode, GLenum type, const void* indirect, GLsizei drawcount, GLsizei stride) { detail::glMultiDrawElementsIndirect(mode, type, indirect, drawcount, stride); }
inline void /*gl*/GetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint* params) { detail::glGetProgramInterfaceiv(program, programInterface, pname, params); }
inline GLuint /*gl*/GetProgramResourceIndex(GLuint program, GLenum programInterface, const GLchar* name) { return detail::glGetProgramResourceIndex(program, programInterface, name); }
inline void /*gl*/GetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, GLchar* name) { detail::glGetProgramResourceName(program, programInterface, index, bufSize, length, name); }
inline void /*gl*/GetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei count, GLsizei* length, GLint* params) { detail::glGetProgramResourceiv(program, programInterface, index, propCount, props, count, length, params); }
inline GLint /*gl*/GetProgramResourceLocation(GLuint program, GLenum programInterface, const GLchar* name) { return detail::glGetProgramResourceLocation(program, programInterface, name); }
inline GLint /*gl*/GetProgramResourceLocationIndex(GLuint program, GLenum programInterface, const GLchar* name) { return detail::glGetProgramResourceLocationIndex(program, programInterface, name); }
inline void /*gl*/ShaderStorageBlockBinding(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding) { detail::glShaderStorageBlockBinding(program, storageBlockIndex, storageBlockBinding); }
inline void /*gl*/TexBufferRange(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) { detail::glTexBufferRange(target, internalformat, buffer, offset, size); }
inline void /*gl*/TexStorage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) { detail::glTexStorage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations); }
inline void /*gl*/TexStorage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) { detail::glTexStorage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations); }
inline void /*gl*/TextureView(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers) { detail::glTextureView(texture, target, origtexture, internalformat, minlevel, numlevels, minlayer, numlayers); }
inline void /*gl*/BindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) { detail::glBindVertexBuffer(bindingindex, buffer, offset, stride); }
inline void /*gl*/VertexAttribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) { detail::glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset); }
inline void /*gl*/VertexAttribIFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) { detail::glVertexAttribIFormat(attribindex, size, type, relativeoffset); }
inline void /*gl*/VertexAttribLFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) { detail::glVertexAttribLFormat(attribindex, size, type, relativeoffset); }
inline void /*gl*/VertexAttribBinding(GLuint attribindex, GLuint bindingindex) { detail::glVertexAttribBinding(attribindex, bindingindex); }
inline void /*gl*/VertexBindingDivisor(GLuint bindingindex, GLuint divisor) { detail::glVertexBindingDivisor(bindingindex, divisor); }
inline void /*gl*/DebugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled) { detail::glDebugMessageControl(source, type, severity, count, ids, enabled); }
inline void /*gl*/DebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf) { detail::glDebugMessageInsert(source, type, id, severity, length, buf); }
inline void /*gl*/DebugMessageCallback(GLDebugCallback callback, const void* userParam) { detail::glDebugMessageCallback(callback, userParam); }
inline GLuint /*gl*/GetDebugMessageLog(GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog) { return detail::glGetDebugMessageLog(count, bufSize, sources, types, ids, severities, lengths, messageLog); }
inline void /*gl*/PushDebugGroup(GLenum source, GLuint id, GLsizei length, const GLchar* message) { detail::glPushDebugGroup(source, id, length, message); }
inline void /*gl*/PopDebugGroup() { detail::glPopDebugGroup(); }
inline void /*gl*/ObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar* label) { detail::glObjectLabel(identifier, name, length, label); }
inline void /*gl*/GetObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei* length, GLchar* label) { detail::glGetObjectLabel(identifier, name, bufSize, length, label); }
inline void /*gl*/ObjectPtrLabel(const void* ptr, GLsizei length, const GLchar* label) { detail::glObjectPtrLabel(ptr, length, label); }
inline void /*gl*/GetObjectPtrLabel(const void* ptr, GLsizei bufSize, GLsizei* length, GLchar* label) { detail::glGetObjectPtrLabel(ptr, bufSize, length, label); }
inline void /*gl*/GetPointerv(GLenum pname, void** params) { detail::glGetPointerv(pname, params); }

// OpenGL 4.4
inline void /*gl*/BufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags) { detail::glBufferStorage(target, size, data, flags); }
inline void /*gl*/ClearTexImage(GLuint texture, GLint level, GLenum format, GLenum type, const void* data) { detail::glClearTexImage(texture, level, format, type, data); }
inline void /*gl*/ClearTexSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* data) { detail::glClearTexSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data); }
inline void /*gl*/BindBuffersBase(GLenum target, GLuint first, GLsizei count, const GLuint* buffers) { detail::glBindBuffersBase(target, first, count, buffers); }
inline void /*gl*/BindBuffersRange(GLenum target, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizeiptr* sizes) { detail::glBindBuffersRange(target, first, count, buffers, offsets, sizes); }
inline void /*gl*/BindTextures(GLuint first, GLsizei count, const GLuint* textures) { detail::glBindTextures(first, count, textures); }
inline void /*gl*/BindSamplers(GLuint first, GLsizei count, const GLuint* samplers) { detail::glBindSamplers(first, count, samplers); }
inline void /*gl*/BindImageTextures(GLuint first, GLsizei count, const GLuint* textures) { detail::glBindImageTextures(first, count, textures); }
inline void /*gl*/BindVertexBuffers(GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides) { detail::glBindVertexBuffers(first, count, buffers, offsets, strides); }

// OpenGL 4.5
inline void /*gl*/ClipControl(GLenum origin, GLenum depth) { detail::glClipControl(origin, depth); }
inline void /*gl*/CreateTransformFeedbacks(GLsizei n, GLuint* ids) { detail::glCreateTransformFeedbacks(n, ids); }
inline void /*gl*/TransformFeedbackBufferBase(GLuint xfb, GLuint index, GLuint buffer) { detail::glTransformFeedbackBufferBase(xfb, index, buffer); }
inline void /*gl*/TransformFeedbackBufferRange(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) { detail::glTransformFeedbackBufferRange(xfb, index, buffer, offset, size); }
inline void /*gl*/GetTransformFeedbackiv(GLuint xfb, GLenum pname, GLint* param) { detail::glGetTransformFeedbackiv(xfb, pname, param); }
inline void /*gl*/GetTransformFeedbacki_v(GLuint xfb, GLenum pname, GLuint index, GLint* param) { detail::glGetTransformFeedbacki_v(xfb, pname, index, param); }
inline void /*gl*/GetTransformFeedbacki64_v(GLuint xfb, GLenum pname, GLuint index, GLint64* param) { detail::glGetTransformFeedbacki64_v(xfb, pname, index, param); }
inline void /*gl*/CreateBuffers(GLsizei n, GLuint* buffers) { detail::glCreateBuffers(n, buffers); }
inline void /*gl*/NamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags) { detail::glNamedBufferStorage(buffer, size, data, flags); }
inline void /*gl*/NamedBufferData(GLuint buffer, GLsizeiptr size, const void* data, GLenum usage) { detail::glNamedBufferData(buffer, size, data, usage); }
inline void /*gl*/NamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) { detail::glNamedBufferSubData(buffer, offset, size, data); }
inline void /*gl*/CopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) { detail::glCopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, size); }
inline void /*gl*/ClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data) { detail::glClearNamedBufferData(buffer, internalformat, format, type, data); }
inline void /*gl*/ClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data) { detail::glClearNamedBufferSubData(buffer, internalformat, offset, size, format, type, data); }
inline void* /*gl*/MapNamedBuffer(GLuint buffer, GLenum access) { return detail::glMapNamedBuffer(buffer, access); }
inline void* /*gl*/MapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access) { return detail::glMapNamedBufferRange(buffer, offset, length, access); }
inline GLboolean /*gl*/UnmapNamedBuffer(GLuint buffer) { return detail::glUnmapNamedBuffer(buffer); }
inline void /*gl*/FlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length) { detail::glFlushMappedNamedBufferRange(buffer, offset, length); }
inline void /*gl*/GetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params) { detail::glGetNamedBufferParameteriv(buffer, pname, params); }
inline void /*gl*/GetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params) { detail::glGetNamedBufferParameteri64v(buffer, pname, params); }
inline void /*gl*/GetNamedBufferPointerv(GLuint buffer, GLenum pname, void** params) { detail::glGetNamedBufferPointerv(buffer, pname, params); }
inline void /*gl*/GetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data) { detail::glGetNamedBufferSubData(buffer, offset, size, data); }
inline void /*gl*/CreateFramebuffers(GLsizei n, GLuint* framebuffers) { detail::glCreateFramebuffers(n, framebuffers); }
inline void /*gl*/NamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) { detail::glNamedFramebufferRenderbuffer(framebuffer, attachment, renderbuffertarget, renderbuffer); }
inline void /*gl*/NamedFramebufferParameteri(GLuint framebuffer, GLenum pname, GLint param) { detail::glNamedFramebufferParameteri(framebuffer, pname, param); }
inline void /*gl*/NamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) { detail::glNamedFramebufferTexture(framebuffer, attachment, texture, level); }
inline void /*gl*/NamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer) { detail::glNamedFramebufferTextureLayer(framebuffer, attachment, texture, level, layer); }
inline void /*gl*/NamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf) { detail::glNamedFramebufferDrawBuffer(framebuffer, buf); }
inline void /*gl*/NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs) { detail::glNamedFramebufferDrawBuffers(framebuffer, n, bufs); }
inline void /*gl*/NamedFramebufferReadBuffer(GLuint framebuffer, GLenum src) { detail::glNamedFramebufferReadBuffer(framebuffer, src); }
inline void /*gl*/InvalidateNamedFramebufferData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments) { detail::glInvalidateNamedFramebufferData(framebuffer, numAttachments, attachments); }
inline void /*gl*/InvalidateNamedFramebufferSubData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height) { detail::glInvalidateNamedFramebufferSubData(framebuffer, numAttachments, attachments, x, y, width, height); }
inline void /*gl*/ClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value) { detail::glClearNamedFramebufferiv(framebuffer, buffer, drawbuffer, value); }
inline void /*gl*/ClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value) { detail::glClearNamedFramebufferuiv(framebuffer, buffer, drawbuffer, value); }
inline void /*gl*/ClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value) { detail::glClearNamedFramebufferfv(framebuffer, buffer, drawbuffer, value); }
inline void /*gl*/ClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) { detail::glClearNamedFramebufferfi(framebuffer, buffer, drawbuffer, depth, stencil); }
inline void /*gl*/BlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) { detail::glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter); }
inline GLenum /*gl*/CheckNamedFramebufferStatus(GLuint framebuffer, GLenum target) { return detail::glCheckNamedFramebufferStatus(framebuffer, target); }
inline void /*gl*/GetNamedFramebufferParameteriv(GLuint framebuffer, GLenum pname, GLint* param) { detail::glGetNamedFramebufferParameteriv(framebuffer, pname, param); }
inline void /*gl*/GetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params) { detail::glGetNamedFramebufferAttachmentParameteriv(framebuffer, attachment, pname, params); }
inline void /*gl*/CreateRenderbuffers(GLsizei n, GLuint* renderbuffers) { detail::glCreateRenderbuffers(n, renderbuffers); }
inline void /*gl*/NamedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height) { detail::glNamedRenderbufferStorage(renderbuffer, internalformat, width, height); }
inline void /*gl*/NamedRenderbufferStorageMultisample(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) { detail::glNamedRenderbufferStorageMultisample(renderbuffer, samples, internalformat, width, height); }
inline void /*gl*/GetNamedRenderbufferParameteriv(GLuint renderbuffer, GLenum pname, GLint* params) { detail::glGetNamedRenderbufferParameteriv(renderbuffer, pname, params); }
inline void /*gl*/CreateTextures(GLenum target, GLsizei n, GLuint* textures) { detail::glCreateTextures(target, n, textures); }
inline void /*gl*/TextureBuffer(GLuint texture, GLenum internalformat, GLuint buffer) { detail::glTextureBuffer(texture, internalformat, buffer); }
inline void /*gl*/TextureBufferRange(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) { detail::glTextureBufferRange(texture, internalformat, buffer, offset, size); }
inline void /*gl*/TextureStorage1D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width) { detail::glTextureStorage1D(texture, levels, internalformat, width); }
inline void /*gl*/TextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) { detail::glTextureStorage2D(texture, levels, internalformat, width, height); }
inline void /*gl*/TextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) { detail::glTextureStorage3D(texture, levels, internalformat, width, height, depth); }
inline void /*gl*/TextureStorage2DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) { detail::glTextureStorage2DMultisample(texture, samples, internalformat, width, height, fixedsamplelocations); }
inline void /*gl*/TextureStorage3DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) { detail::glTextureStorage3DMultisample(texture, samples, internalformat, width, height, depth, fixedsamplelocations); }
inline void /*gl*/TextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels) { detail::glTextureSubImage1D(texture, level, xoffset, width, format, type, pixels); }
inline void /*gl*/TextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) { detail::glTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels); }
inline void /*gl*/TextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels) { detail::glTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels); }
inline void /*gl*/CompressedTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data) { detail::glCompressedTextureSubImage1D(texture, level, xoffset, width, format, imageSize, data); }
inline void /*gl*/CompressedTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) { detail::glCompressedTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, imageSize, data); }
inline void /*gl*/CompressedTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data) { detail::glCompressedTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data); }
inline void /*gl*/CopyTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) { detail::glCopyTextureSubImage1D(texture, level, xoffset, x, y, width); }
inline void /*gl*/CopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) { detail::glCopyTextureSubImage2D(texture, level, xoffset, yoffset, x, y, width, height); }
inline void /*gl*/CopyTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) { detail::glCopyTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, x, y, width, height); }
inline void /*gl*/TextureParameterf(GLuint texture, GLenum pname, GLfloat param) { detail::glTextureParameterf(texture, pname, param); }
inline void /*gl*/TextureParameterfv(GLuint texture, GLenum pname, const GLfloat* param) { detail::glTextureParameterfv(texture, pname, param); }
inline void /*gl*/TextureParameteri(GLuint texture, GLenum pname, GLint param) { detail::glTextureParameteri(texture, pname, param); }
inline void /*gl*/TextureParameterIiv(GLuint texture, GLenum pname, const GLint* params) { detail::glTextureParameterIiv(texture, pname, params); }
inline void /*gl*/TextureParameterIuiv(GLuint texture, GLenum pname, const GLuint* params) { detail::glTextureParameterIuiv(texture, pname, params); }
inline void /*gl*/TextureParameteriv(GLuint texture, GLenum pname, const GLint* param) { detail::glTextureParameteriv(texture, pname, param); }
inline void /*gl*/GenerateTextureMipmap(GLuint texture) { detail::glGenerateTextureMipmap(texture); }
inline void /*gl*/BindTextureUnit(GLuint unit, GLuint texture) { detail::glBindTextureUnit(unit, texture); }
inline void /*gl*/GetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels) { detail::glGetTextureImage(texture, level, format, type, bufSize, pixels); }
inline void /*gl*/GetCompressedTextureImage(GLuint texture, GLint level, GLsizei bufSize, void* pixels) { detail::glGetCompressedTextureImage(texture, level, bufSize, pixels); }
inline void /*gl*/GetTextureLevelParameterfv(GLuint texture, GLint level, GLenum pname, GLfloat* params) { detail::glGetTextureLevelParameterfv(texture, level, pname, params); }
inline void /*gl*/GetTextureLevelParameteriv(GLuint texture, GLint level, GLenum pname, GLint* params) { detail::glGetTextureLevelParameteriv(texture, level, pname, params); }
inline void /*gl*/GetTextureParameterfv(GLuint texture, GLenum pname, GLfloat* params) { detail::glGetTextureParameterfv(texture, pname, params); }
inline void /*gl*/GetTextureParameterIiv(GLuint texture, GLenum pname, GLint* params) { detail::glGetTextureParameterIiv(texture, pname, params); }
inline void /*gl*/GetTextureParameterIuiv(GLuint texture, GLenum pname, GLuint* params) { detail::glGetTextureParameterIuiv(texture, pname, params); }
inline void /*gl*/GetTextureParameteriv(GLuint texture, GLenum pname, GLint* params) { detail::glGetTextureParameteriv(texture, pname, params); }
inline void /*gl*/CreateVertexArrays(GLsizei n, GLuint* arrays) { detail::glCreateVertexArrays(n, arrays); }
inline void /*gl*/DisableVertexArrayAttrib(GLuint vaobj, GLuint index) { detail::glDisableVertexArrayAttrib(vaobj, index); }
inline void /*gl*/EnableVertexArrayAttrib(GLuint vaobj, GLuint index) { detail::glEnableVertexArrayAttrib(vaobj, index); }
inline void /*gl*/VertexArrayElementBuffer(GLuint vaobj, GLuint buffer) { detail::glVertexArrayElementBuffer(vaobj, buffer); }
inline void /*gl*/VertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) { detail::glVertexArrayVertexBuffer(vaobj, bindingindex, buffer, offset, stride); }
inline void /*gl*/VertexArrayVertexBuffers(GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides) { detail::glVertexArrayVertexBuffers(vaobj, first, count, buffers, offsets, strides); }
inline void /*gl*/VertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex) { detail::glVertexArrayAttribBinding(vaobj, attribindex, bindingindex); }
inline void /*gl*/VertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) { detail::glVertexArrayAttribFormat(vaobj, attribindex, size, type, normalized, relativeoffset); }
inline void /*gl*/VertexArrayAttribIFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) { detail::glVertexArrayAttribIFormat(vaobj, attribindex, size, type, relativeoffset); }
inline void /*gl*/VertexArrayAttribLFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) { detail::glVertexArrayAttribLFormat(vaobj, attribindex, size, type, relativeoffset); }
inline void /*gl*/VertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor) { detail::glVertexArrayBindingDivisor(vaobj, bindingindex, divisor); }
inline void /*gl*/GetVertexArrayiv(GLuint vaobj, GLenum pname, GLint* param) { detail::glGetVertexArrayiv(vaobj, pname, param); }
inline void /*gl*/GetVertexArrayIndexediv(GLuint vaobj, GLuint index, GLenum pname, GLint* param) { detail::glGetVertexArrayIndexediv(vaobj, index, pname, param); }
inline void /*gl*/GetVertexArrayIndexed64iv(GLuint vaobj, GLuint index, GLenum pname, GLint64* param) { detail::glGetVertexArrayIndexed64iv(vaobj, index, pname, param); }
inline void /*gl*/CreateSamplers(GLsizei n, GLuint* samplers) { detail::glCreateSamplers(n, samplers); }
inline void /*gl*/CreateProgramPipelines(GLsizei n, GLuint* pipelines) { detail::glCreateProgramPipelines(n, pipelines); }
inline void /*gl*/CreateQueries(GLenum target, GLsizei n, GLuint* ids) { detail::glCreateQueries(target, n, ids); }
inline void /*gl*/GetQueryBufferObjecti64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset) { detail::glGetQueryBufferObjecti64v(id, buffer, pname, offset); }
inline void /*gl*/GetQueryBufferObjectiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset) { detail::glGetQueryBufferObjectiv(id, buffer, pname, offset); }
inline void /*gl*/GetQueryBufferObjectui64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset) { detail::glGetQueryBufferObjectui64v(id, buffer, pname, offset); }
inline void /*gl*/GetQueryBufferObjectuiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset) { detail::glGetQueryBufferObjectuiv(id, buffer, pname, offset); }
inline void /*gl*/MemoryBarrierByRegion(GLbitfield barriers) { detail::glMemoryBarrierByRegion(barriers); }
inline void /*gl*/GetTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void* pixels) { detail::glGetTextureSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, bufSize, pixels); }
inline void /*gl*/GetCompressedTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void* pixels) { detail::glGetCompressedTextureSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth, bufSize, pixels); }
inline GLenum /*gl*/GetGraphicsResetStatus() { return detail::glGetGraphicsResetStatus(); }
inline void /*gl*/GetnCompressedTexImage(GLenum target, GLint lod, GLsizei bufSize, void* pixels) { detail::glGetnCompressedTexImage(target, lod, bufSize, pixels); }
inline void /*gl*/GetnTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels) { detail::glGetnTexImage(target, level, format, type, bufSize, pixels); }
inline void /*gl*/GetnUniformdv(GLuint program, GLint location, GLsizei bufSize, GLdouble* params) { detail::glGetnUniformdv(program, location, bufSize, params); }
inline void /*gl*/GetnUniformfv(GLuint program, GLint location, GLsizei bufSize, GLfloat* params) { detail::glGetnUniformfv(program, location, bufSize, params); }
inline void /*gl*/GetnUniformiv(GLuint program, GLint location, GLsizei bufSize, GLint* params) { detail::glGetnUniformiv(program, location, bufSize, params); }
inline void /*gl*/GetnUniformuiv(GLuint program, GLint location, GLsizei bufSize, GLuint* params) { detail::glGetnUniformuiv(program, location, bufSize, params); }
inline void /*gl*/ReadnPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void* data) { detail::glReadnPixels(x, y, width, height, format, type, bufSize, data); }
inline void /*gl*/GetnMapdv(GLenum target, GLenum query, GLsizei bufSize, GLdouble* v) { detail::glGetnMapdv(target, query, bufSize, v); }
inline void /*gl*/GetnMapfv(GLenum target, GLenum query, GLsizei bufSize, GLfloat* v) { detail::glGetnMapfv(target, query, bufSize, v); }
inline void /*gl*/GetnMapiv(GLenum target, GLenum query, GLsizei bufSize, GLint* v) { detail::glGetnMapiv(target, query, bufSize, v); }
inline void /*gl*/GetnPixelMapfv(GLenum map, GLsizei bufSize, GLfloat* values) { detail::glGetnPixelMapfv(map, bufSize, values); }
inline void /*gl*/GetnPixelMapuiv(GLenum map, GLsizei bufSize, GLuint* values) { detail::glGetnPixelMapuiv(map, bufSize, values); }
inline void /*gl*/GetnPixelMapusv(GLenum map, GLsizei bufSize, GLushort* values) { detail::glGetnPixelMapusv(map, bufSize, values); }
inline void /*gl*/GetnPolygonStipple(GLsizei bufSize, GLubyte* pattern) { detail::glGetnPolygonStipple(bufSize, pattern); }
inline void /*gl*/GetnColorTable(GLenum target, GLenum format, GLenum type, GLsizei bufSize, void* table) { detail::glGetnColorTable(target, format, type, bufSize, table); }
inline void /*gl*/GetnConvolutionFilter(GLenum target, GLenum format, GLenum type, GLsizei bufSize, void* image) { detail::glGetnConvolutionFilter(target, format, type, bufSize, image); }
inline void /*gl*/GetnSeparableFilter(GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, void* row, GLsizei columnBufSize, void* column, void* span) { detail::glGetnSeparableFilter(target, format, type, rowBufSize, row, columnBufSize, column, span); }
inline void /*gl*/GetnHistogram(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void* values) { detail::glGetnHistogram(target, reset, format, type, bufSize, values); }
inline void /*gl*/GetnMinmax(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void* values) { detail::glGetnMinmax(target, reset, format, type, bufSize, values); }
inline void /*gl*/TextureBarrier() { detail::glTextureBarrier(); }

//============================================================================//

} // namespace sq::gl
