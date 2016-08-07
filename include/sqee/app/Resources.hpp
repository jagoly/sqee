#pragma once

#include <sqee/assert.hpp>
#include <sqee/builtins.hpp>

namespace sq {

template <class T> struct Resource {
    void decrement() { if (--count == 0u) uptr.reset(); }
    bool loaded() const { return uptr != nullptr; }
    ushort count = 0u; unique_ptr<T> uptr;
};

template <class T> struct Handle {
    Handle() : resource(nullptr) {} ~Handle() { if (resource) resource->decrement(); }
    Handle(const Handle& _other) { if ((resource = _other.resource)) ++resource->count; }
    Handle& operator=(Handle _other) { std::swap(resource, _other.resource); return *this; }
    Handle(Resource<T>& _resource) { resource = &_resource; ++resource->count; }

    void set_null() { this->~Handle(); resource = nullptr; }
    bool check() const { return resource != nullptr; }

    bool operator==(const Handle& _other) const { return resource == _other.resource; }
    bool operator!=(const Handle& _other) const { return resource != _other.resource; }
    bool operator<(const Handle& _other) const { return resource < _other.resource; }

    const T* operator->() const {
        SQASSERT(resource != nullptr, "");
        SQASSERT(resource->loaded(), "");
        return resource->uptr.get();
    }

    const T& get() const {
        SQASSERT(resource != nullptr, "");
        SQASSERT(resource->loaded(), "");
        return *resource->uptr;
    }

    private: Resource<T>* resource;
};

string& static_path();

}
