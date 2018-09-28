// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

namespace sq {

//============================================================================//

/// OpenGL Shader Program Object.
class SQEE_API Program final : private MoveOnly
{
public: //====================================================//

    Program(); ///< Constructor.

    Program(Program&& other); ///< Move Constructor.

    Program& operator=(Program&& other); ///< Move Assignment.

    ~Program(); ///< Destructor.

    //--------------------------------------------------------//

    /// Load the vertex shader stage from a string.
    void load_vertex(const String& source, const String& path = "");

    /// Load the geometry shader stage from a string.
    void load_geometry(const String& source, const String& path = "");

    /// Load the fragment shader stage from a string.
    void load_fragment(const String& source, const String& path = "");

    /// Link shader stages together, preparing the program for use.
    void link_program_stages();

    /// Create a program from vertex and fragment sources.
    void create(const String& vertex, const String& fragment);

    //--------------------------------------------------------//

    /// Update a uniform using it's location.
    void update(GLint location, int sca) const;

    /// Update a uniform using it's location.
    void update(GLint location, uint sca) const;

    /// Update a uniform using it's location.
    void update(GLint location, float sca) const;

    //--------------------------------------------------------//

    /// Update a uniform using it's location.
    void update(GLint location, Vec2I vec) const;

    /// Update a uniform using it's location.
    void update(GLint location, Vec2U vec) const;

    /// Update a uniform using it's location.
    void update(GLint location, Vec2F vec) const;

    //--------------------------------------------------------//

    /// Update a uniform using it's location.
    void update(GLint location, Vec3I vec) const;

    /// Update a uniform using it's location.
    void update(GLint location, Vec3U vec) const;

    /// Update a uniform using it's location.
    void update(GLint location, Vec3F vec) const;

    //--------------------------------------------------------//

    /// Update a uniform using it's location.
    void update(GLint location, Vec4I vec) const;

    /// Update a uniform using it's location.
    void update(GLint location, Vec4U vec) const;

    /// Update a uniform using it's location.
    void update(GLint location, Vec4F vec) const;

    //--------------------------------------------------------//

    /// Update a uniform using it's location.
    void update(GLint location, const Mat3F& mat) const;

    /// Update a uniform using it's location.
    void update(GLint location, const Mat34F& mat) const;

    /// Update a uniform using it's location.
    void update(GLint location, const Mat4F& mat) const;

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

private: //===================================================//

    class Context& mContext;

    GLuint mVertexShader = 0u;
    GLuint mGeometryShader = 0u;
    GLuint mFragmentShader = 0u;

    GLuint mHandle = 0u;
};

//============================================================================//

struct ProgramKey final
{
    String vertexPath, vertexDefines;
    String fragmentPath, fragmentDefines;
};

inline bool operator==(const ProgramKey& a, const ProgramKey& b)
{
    return a.fragmentDefines == b.fragmentDefines && b.fragmentPath == b.fragmentPath
        && a.vertexDefines == b.vertexDefines && a.vertexPath == b.vertexPath;
}

//============================================================================//

} // namespace sq

//============================================================================//

namespace std {

template<> struct hash<sq::ProgramKey>
{
    size_t operator()(const sq::ProgramKey& key) const
    {
        const auto hash_func = std::hash<std::string>();

        const size_t vp = hash_func(key.vertexPath);
        const size_t vd = hash_func(key.vertexDefines);
        const size_t fp = hash_func(key.fragmentPath);
        const size_t fd = hash_func(key.fragmentDefines);

        const auto combine_func = [](size_t a, size_t b) -> size_t
        { return a ^ (b + 0x9e3779b9u + (a << 6u) + (a >> 2u)); };

        return combine_func(combine_func(vp, vd), combine_func(fp, fd));
    }
};

} // namespace std
