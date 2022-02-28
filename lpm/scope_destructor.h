#pragma once
#include <memory>
#include <functional>

namespace LPM {
    // Call a destructor when something goes out of scope
    template <class T>
    class scope_destructor {
    private:
        T _object;
        std::function<void(T)> _destructor;
    public:
        scope_destructor(T object, std::function<void(T)> destructor)
            : _object(object), _destructor(destructor) {}
        ~scope_destructor() {  _destructor(_object); LPM_PRINT_DEBUG("scope_destructor went out of scope"); }

        T& get() { return _object; }
    };
}