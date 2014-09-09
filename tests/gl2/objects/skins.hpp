#pragma once

#include <string>
#include <vector>
#include <memory>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/resources/texholder.hpp>

#define MODELS_DIR "res/models/"

namespace sqt {

struct Skin {
    void load(std::string& _filePath, sq::TexHolderBasic* _texHolder);

    sq::Texture::Ptr texNorm;
    sq::Texture::Ptr texDiff;
    sq::Texture::Ptr texSpec;
    int ambi = 0;
    bool alpha = false;
};

}
