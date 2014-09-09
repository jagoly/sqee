#pragma once

#include <string>
#include <vector>
#include <memory>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>

#include "meshes.hpp"
#include "skins.hpp"

namespace sqt {

struct Model {
    bool load_from_dir(std::string _name, std::string _skin, sq::TexHolderBasic* _texHolder);

    MeshPtr mesh;
    Skin skin;
};
typedef std::shared_ptr<Model> ModelPtr;

}
