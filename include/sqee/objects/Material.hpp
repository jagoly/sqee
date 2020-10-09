// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/misc/ResourceCache.hpp>

namespace sq {

//============================================================================//

/// The SQEE Material class.
class SQEE_API Material final : private MoveOnly
{
public: //====================================================//

    using ProgramCache = ResourceCache<JsonValue, Program>;
    using TextureCache = ResourceCache<String, Texture2D>;

    /// Load the material from a json object.
    void load_from_json(const JsonValue& json, ProgramCache& programs, TextureCache& textures);

    /// Bind the material to the context.
    void apply_to_context(Context& context) const;

private: //===================================================//

    //using TextureValue = std::variant<Handle<String, Texture2D>, Handle<String, TextureArray>, Handle<String, TextureCube>>;
    using UniformValue = std::variant
    <
        int, float, Vec2I, Vec2F,
        Vec3I, Vec3F, Vec4I, Vec4F,
        Handle<String, Texture2D>,
        Handle<String, TextureArray>,
        Handle<String, TextureCube>
    >;

    CullFace mCullface;

    Handle<JsonValue, Program> mProgram;

    //std::vector<std::pair<uint, TextureValue>> mTextures;
    //std::vector<std::pair<int, UniformValue>> mUniforms;

    std::vector<std::pair<int, UniformValue>> mUniforms;
};

//============================================================================//

} // namespace sq
