#include <iostream>

#include <text/font.hpp>

namespace sq {

Font::Ptr create_font(const std::string& _fPath, FT_Library& _ftLib, int _size) {
    Font::Ptr ptr(new Font());

    int error = FT_New_Face(_ftLib, _fPath.c_str(), 0, &ptr->face);
    if (error) std::cout << "ERROR: Failed to load font from \"" << _fPath << "\"" << std::endl;

    FT_Set_Pixel_Sizes(ptr->face, 0, _size);

    return ptr;
}

}

using namespace sq;

void Font::set_size(int _size) {
    FT_Set_Pixel_Sizes(face, 0, _size);
}
