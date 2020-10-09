#include <sqee/objects/Material.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/misc/Json.hpp>

using namespace sq;

//============================================================================//

void Material::load_from_json(const JsonValue& json, ProgramCache& programs, TextureCache& textures)
{
    const String& cullface = json.at("cullface");
    if (cullface == "disable") mCullface = CullFace::Disable;
    else if (cullface == "back") mCullface = CullFace::Back;
    else if (cullface == "front") mCullface = CullFace::Front;
    else log_error("cullface '{}'", cullface);

    mProgram = programs.acquire(json.at("program"));

//    for (const auto& [key, value] : json.at("textures").items())
//    {
//        const auto info = mProgram->query_sampler_info(key);
//        if (!info.has_value()) log_error("program does not have sampler '{}'", key);

//        if      (info->type == gl::SAMPLER_2D)       mTextures.emplace_back(info->binding, textures.acquire(value));
//        else if (info->type == gl::SAMPLER_2D_ARRAY) log_error("todo: texarrays in materials");
//        else if (info->type == gl::SAMPLER_CUBE)     log_error("todo: texcubes in materials");

//        else log_error("unsupported type for sampler '{}'", key);
//    }

    for (const auto& [key, value] : json.at("uniforms").items())
    {
        const auto info = mProgram->query_uniform_info(key);
        if (!info.has_value()) log_error("program does not have uniform '{}'", key);

        if      (info->type == gl::INT)        mUniforms.emplace_back(info->location, int(value));
        else if (info->type == gl::FLOAT)      mUniforms.emplace_back(info->location, float(value));
        else if (info->type == gl::INT_VEC2)   mUniforms.emplace_back(info->location, Vec2I(value));
        else if (info->type == gl::FLOAT_VEC2) mUniforms.emplace_back(info->location, Vec2F(value));
        else if (info->type == gl::INT_VEC3)   mUniforms.emplace_back(info->location, Vec3I(value));
        else if (info->type == gl::FLOAT_VEC3) mUniforms.emplace_back(info->location, Vec3F(value));
        else if (info->type == gl::INT_VEC4)   mUniforms.emplace_back(info->location, Vec4I(value));
        else if (info->type == gl::FLOAT_VEC4) mUniforms.emplace_back(info->location, Vec4F(value));

        else if (info->type == gl::SAMPLER_2D)       mUniforms.emplace_back(int(info->binding), textures.acquire(value));
        else if (info->type == gl::SAMPLER_2D_ARRAY) log_error("todo: texarrays in materials");
        else if (info->type == gl::SAMPLER_CUBE)     log_error("todo: texcubes in materials");

        else log_error("unsupported type for uniform '{}'", key);
    }
}

//============================================================================//

void Material::apply_to_context(Context& context) const
{
    context.set_state(mCullface);
    context.bind_program(mProgram.get());

    for (const auto& item : mUniforms)
    {
        const auto visitor = [&](const auto& value)
        {
            using Type = std::remove_reference_t<decltype(value)>;
            if constexpr (std::is_arithmetic_v<Type> || detail::is_vector_v<Type>)
                mProgram->update(int(item.first), value);
            else
                context.bind_texture(*value, uint(item.first));
        };
        std::visit(visitor, item.second);
    }

    //for (const auto& [slot, value] : mTextures)
    //    if (auto ptr = std::get_if<Handle<String, Texture2D>>(&value))
    //        context.bind_texture(ptr->get(), slot);
    //
    //for (const auto& [location, value] : mUniforms)
    //    if (auto ptr = std::get_if<Vec3F>(&value))
    //        mProgram->update(location, *ptr);
}
