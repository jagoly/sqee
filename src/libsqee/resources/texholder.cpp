#include <iostream>

#include <resources/texholder.hpp>

using namespace sq;

template <class Tk, class Tv>
Tv& TexHolder<Tk, Tv>::add(Tk _id) {
    if (has(_id)) {
        std::cout << "WARNING: Overwriting existing texture in map" << std::endl;
        texMap.erase(_id);
    }
    return texMap[_id];
}

template <class Tk, class Tv>
Tv& TexHolder<Tk, Tv>::get(Tk _id) {
    return texMap.at(_id);
}

template <class Tk, class Tv>
void TexHolder<Tk, Tv>::set(Tk _id, Tv _val) {
    texMap[_id] = _val;
}

template <class Tk, class Tv>
bool TexHolder<Tk, Tv>::has(Tk _id) {
    if (texMap.count(_id))
        return true;
    return false;
}

template <class Tk, class Tv>
Tv& TexHolder<Tk, Tv>::operator[](const Tk& _key) {
    return get(_key);
}

template class TexHolder<std::string, Texture::Ptr>;
