// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/gl/SafeEnums.hpp>

namespace sq {

//============================================================================//

/// OpenGL Shader Program Object.
class SQEE_API Program final : private MoveOnly
{
public: //====================================================//

    struct UniformInfo
    {
        GLenum type;
        GLint location;
    };

    struct SamplerInfo
    {
        GLenum type;
        GLuint binding;
    };

    //--------------------------------------------------------//

    Program() = default; ///< Constructor.

    Program(Program&& other) noexcept; ///< Move Constructor.

    Program& operator=(Program&& other) noexcept; ///< Move Assignment.

    ~Program() noexcept; ///< Destructor.

    //--------------------------------------------------------//

    /// Load one shader stage from a string.
    void load_shader(ShaderStage stage, StringView source, StringView path = {});

    /// Link shader stages together, preparing the program for use.
    void link_program_stages();

    /// Create a program from vertex and fragment sources.
    void create(StringView vertex, StringView fragment);

    //--------------------------------------------------------//

    /// Query the type and location of a uniform.
    std::optional<UniformInfo> query_uniform_info(const String& name) const;

    /// Query the type and binding of a sampler.
    std::optional<SamplerInfo> query_sampler_info(const String& name) const;

    //--------------------------------------------------------//

    void update(GLint location, int value) const;
    void update(GLint location, uint value) const;
    void update(GLint location, float value) const;

    void update(GLint location, Vec2I vec) const;
    void update(GLint location, Vec2U vec) const;
    void update(GLint location, Vec2F vec) const;

    void update(GLint location, Vec3I vec) const;
    void update(GLint location, Vec3U vec) const;
    void update(GLint location, Vec3F vec) const;

    void update(GLint location, Vec4I vec) const;
    void update(GLint location, Vec4U vec) const;
    void update(GLint location, Vec4F vec) const;

    void update(GLint location, const Mat3F& mat) const;
    void update(GLint location, const Mat34F& mat) const;
    void update(GLint location, const Mat4F& mat) const;

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

private: //===================================================//

    GLuint mVertexShader = 0u;
    GLuint mGeometryShader = 0u;
    GLuint mFragmentShader = 0u;

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
