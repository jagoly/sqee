#pragma once

#include <iostream>
#include <map>

#include <gl/gl.hpp>
#include <gl/textures.hpp>

namespace sq {

template <class Tk, class Tv>
class ResHolder {
public:
    typedef typename std::map<Tk, Tv>::iterator iterator;
    iterator begin() { return map.begin(); }
    iterator end() { return map.end(); }

    typedef std::pair<const Tk, Tv> Pair;

    Tv&  add(Tk _id) {
        if (has(_id)) {
            std::cout << "WARNING: Overwriting existing resource in map" << std::endl;
            map.erase(_id);
        }
        return map[_id];
    }

    Tv&  get(Tk _id) {
        return map.at(_id);
    }

    void set(Tk _id, Tv _val) {
        map[_id] = _val;
    }

    bool has(Tk _id) {
        if (map.count(_id))
            return true;
        return false;
    }

    void del(Tk _id) {
        // TODO
    }

    Tv& operator[](const Tk& _key) {
        return get(_key);
    }

    std::map<Tk, Tv> map;
};

typedef ResHolder<std::string, Texture::Ptr> TexHolder;

}
