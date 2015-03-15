#pragma once
#include <sqee/forward.hpp>

#include <forward_list>
#include <functional>
#include <map>
#include <set>
#include <vector>

namespace sqt {

class Cell;

enum class ObjType {
    Model, Reflector, Liquid, Light
};

template<class T> struct ObjTraits {
    template<T> static constexpr ObjType type();
};

struct ObjSpec {
    ObjSpec(const string& _name, ObjType _type, Cell& _cell);

    const ObjType type;
    const string name;
    Cell& cell;

    void parse_line(const std::vector<string>& _line);

    std::map<string, std::vector<bool>> bMap;
    std::map<string, std::vector<int>> iMap;
    std::map<string, std::vector<float>> fMap;
    std::map<string, std::vector<string>> sMap;
    std::set<string> flagSet;
};


class Object : NonCopyable {
public:
    Object(const ObjSpec& _spec);

    const ObjType type;
    const string name;
    Cell& cell;

    virtual void tick() {}
    virtual void calc(double _accum) {}
};


template<class T, class IterT>
std::forward_list<T*> filter_objs(const IterT& _begin, const IterT& _end,
                                  std::function<bool(const T&)> _func) {
    std::forward_list<T*> retList;
    for (auto iter = _begin; iter != _end; iter++) {
        if ((*iter)->type != ObjTraits<T>::type()) continue;
        if (_func(static_cast<T&>(**iter)))
            retList.push_front(static_cast<T*>(*iter));

    }

    return retList;
}

}
