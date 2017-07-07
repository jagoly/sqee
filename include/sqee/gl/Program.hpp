#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq {

//============================================================================//

class Context; // Forward Declaration

//============================================================================//

/// OpenGL Shader Program Object.
class Program final : public MoveOnly
{
public: //====================================================//

    Program(); ///< Constructor.

    Program(Program&& other); ///< Move Constructor.

    Program& operator=(Program&& other); ///< Move Assignment.

    ~Program(); ///< Destructor.

    //--------------------------------------------------------//

    /// Load the vertex shader stage from a string.
    void load_vertex(const string& source, const string& path = "");

    /// Load the geometry shader stage from a string.
    void load_geometry(const string& source, const string& path = "");

    /// Load the fragment shader stage from a string.
    void load_fragment(const string& source, const string& path = "");

    /// Link shader stages together, preparing the program for use.
    void link_program_stages();

    /// Create a program from vertex and fragment sources.
    void create(const string& vertex, const string& fragment);

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

    Context& mContext;

    GLuint mVertexShader = 0u;
    GLuint mGeometryShader = 0u;
    GLuint mFragmentShader = 0u;

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
