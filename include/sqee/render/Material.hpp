#pragma once

#include <map>

#include <sqee/misc/ResourceCache.hpp>

#include <sqee/gl/Textures.hpp>

namespace sq {

//============================================================================//

/// The SQEE Material class.
class SQEE_API Material final : private MoveOnly
{
public: //====================================================//

    Material(const String& path, ResourceCache<String, Texture2D>& textures);

    /// Move Constructor
    Material(Material&& other);

    //--------------------------------------------------------//

    bool has_diffuse_texture()  const { return mDiffuseTexture.check(); }
    bool has_normal_texture()   const { return mNormalTexture.check(); }
    bool has_specular_texture() const { return mSpecularTexture.check(); }
    bool has_mask_texture()     const { return mMaskTexture.check(); }

    const Handle<String, Texture2D>& get_diffuse_handle()  const { return mDiffuseTexture; }
    const Handle<String, Texture2D>& get_normal_handle()   const { return mNormalTexture; }
    const Handle<String, Texture2D>& get_specular_handle() const { return mSpecularTexture; }
    const Handle<String, Texture2D>& get_mask_handle()     const { return mMaskTexture; }

    const Texture2D& get_diffuse_texture()  const { return mDiffuseTexture.get(); }
    const Texture2D& get_normal_texture()   const { return mNormalTexture.get(); }
    const Texture2D& get_specular_texture() const { return mSpecularTexture.get(); }
    const Texture2D& get_mask_texture()     const { return mMaskTexture.get(); }

    Vec3F get_diffuse_colour()  const { return mDiffuseColour; }
    Vec3F get_specular_colour() const { return mSpecularColour; }

private: //===================================================//

    Handle<String, Texture2D> mDiffuseTexture;
    Handle<String, Texture2D> mNormalTexture;
    Handle<String, Texture2D> mSpecularTexture;
    Handle<String, Texture2D> mMaskTexture;

    Vec3F mDiffuseColour { 1.f, 1.f, 1.f };
    Vec3F mSpecularColour { 1.f, 1.f, 1.f };
};

//============================================================================//

} // namespace sq
