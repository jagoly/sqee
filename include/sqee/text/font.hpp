#pragma once
#include <memory>

#include <ft2build.h>

#include <app/application.hpp>
#include <misc/containers.hpp>
// ^ need to make fonts follow conventions of other sqee resources

namespace sq {

class Font {
public:
    typedef std::shared_ptr<Font> Ptr;

    FT_Library* ftLib;
    FT_Face face;

    void set_size(int _size);
};

Font::Ptr create_font(const std::string& _fPath, FT_Library& _ftLib, int _size);

}
