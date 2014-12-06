#include <text/font.hpp>

using namespace sq;

Font::Ptr sq::create_font(const std::string& _fPath, FT_Library& _ftLib, int _size) {
    Font::Ptr ptr(new Font());

    std::string fPath = SQ_FONTS + _fPath + ".ttf";

    int error = FT_New_Face(_ftLib, fPath.c_str(), 0, &ptr->face);
    if (error) std::cout << "ERROR: Failed to load font from \"" << fPath << "\"" << std::endl;

    FT_Set_Pixel_Sizes(ptr->face, 0, _size);

    return ptr;
}

void Font::set_size(int _size) {
    FT_Set_Pixel_Sizes(face, 0, _size);
}
