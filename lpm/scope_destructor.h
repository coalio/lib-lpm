#pragma once
#include <memory>
#include <functional>
#include "macros.h"

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
            if (_object != nullptr) {
                LPM_PRINT_DEBUG("scope_destructor initialized for object " << _object);
            }
        }

        ~scope_destructor() {
            if (cancelled) {
                return;
            }

            if (_object != nullptr) {
                _destructor(_object);
                LPM_PRINT_DEBUG("scope_destructor called for object " << _object);
            }
        }

        inline T& get() { return _object; }
        inline void cancel() { cancelled = true; }
    };
}