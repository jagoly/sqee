#pragma once

#include <map>

#include <gl/gl.hpp>
#include <gl/textures.hpp>

namespace sq {

template <class Tk, class Tv>
class TexHolder {
public:
    Tv&  add(Tk _id);
    Tv&  get(Tk _id);
    void set(Tk _id, Tv _val);
    bool has(Tk _id);
    void del(Tk _id);

    Tv& operator[](const Tk& _key);

    std::map<Tk, Tv> texMap;
};

typedef TexHolder<std::string, Texture::Ptr> TexHolderBasic;

}
