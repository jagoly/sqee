#include <fontholder.hpp>
#include <staticfont.hpp>

namespace sq {

FontHolder::FontHolder() {
    fontMap["static"] = load_staticfont();
}

sf::Font& FontHolder::get_font(std::string strId) {
    return fontMap[strId];
}

void FontHolder::add_font(std::string strId, std::string strPath) {
    fontMap[strId].loadFromFile(strPath);
}

}
