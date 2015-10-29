#include <sqee/app/Resources.hpp>

#include <sqee/gl/Textures.hpp>
#include <sqee/physics/PhysObject.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

using namespace sq;

string& sq::static_path() {
    static string path;
    return path;
}

ResHolder<Texture2D>& sq::static_Texture2D() {
    static ResHolder<Texture2D> holder;
    return holder;
}

ResHolder<PhysObject>& sq::static_PhysObject() {
    static ResHolder<PhysObject> holder;
    return holder;
}

ResHolder<Armature>& sq::static_Armature() {
    static ResHolder<Armature> holder;
    return holder;
}

ResHolder<Mesh>& sq::static_Mesh() {
    static ResHolder<Mesh> holder;
    return holder;
}

ResHolder<Skin>& sq::static_Skin() {
    static ResHolder<Skin> holder;
    return holder;
}
