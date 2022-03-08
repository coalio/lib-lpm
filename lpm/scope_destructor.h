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
        bool cancelled;
    public:
        scope_destructor(T object, std::function<void(T)> destructor)
        : _object(object), _destructor(destructor) {
            LPM_PRINT_DEBUG("scope_destructor initialized for object " << object);
        }

        ~scope_destructor() {
            if (cancelled) {
                return;
            }

            _destructor(_object);
            LPM_PRINT_DEBUG("scope_destructor called _destructor on " << _object);
        }

        T& get() { return _object; }
        void cancel() { cancelled = true; }
    };
}