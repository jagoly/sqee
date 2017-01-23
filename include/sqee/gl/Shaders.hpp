#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

namespace sq {

//============================================================================//

class Context; // Forward Declaration

//============================================================================//

/// OpenGL Single Stage Shader
class Shader final : public MoveOnly
{
public:

    //========================================================//

    enum class Stage { Vertex, Geometry, Fragment };

    //========================================================//

    /// Constructor
    Shader(Stage stage);

    /// Move Constructor
    Shader(Shader&& other);

    /// Destructor
    ~Shader();

    //========================================================//

    /// Add a named shader uniform. Call before load().
    void add_uniform(const string& name, uint count = 1u);

    /// Load a shader from a string
    void load(const string& source);

    /// Load a shader from a string and set a path for debug
    void load(const string& source, const string& path);

    /// Update a uniform with a previously added name
    template <class T>
    void update(const string& name, const T& value) const;

    //========================================================//

    /// Get the OpenGL object handle
    GLuint get_handle() const { return mHandle; }

private:

    //========================================================//

    GLenum mShaderType = 0u;

    struct Uniform { uint count; GLint id; };
    std::unordered_map<string, Uniform> mUniforms;

    GLuint mHandle = 0u;
    Context& mContext;
};

//============================================================================//

} // namespace sq
