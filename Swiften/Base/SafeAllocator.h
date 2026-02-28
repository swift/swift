/*
 * Copyright (c) 2011-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <algorithm>
#include <vector>

#include <Swiften/Base/API.h>

namespace Swift {
    SWIFTEN_API void secureZeroMemory(char* memory, size_t numberOfBytes);

    template<typename T>
    class SWIFTEN_API SafeAllocator : public std::allocator<T> {
        public:
            template <class U> struct rebind {
                typedef SafeAllocator<U> other;
            };

            SafeAllocator() SWIFTEN_NOEXCEPT {}
            SafeAllocator(const SafeAllocator&) SWIFTEN_NOEXCEPT : std::allocator<T>() {}
            template <class U> SafeAllocator(const SafeAllocator<U>&) SWIFTEN_NOEXCEPT {}
            ~SafeAllocator() SWIFTEN_NOEXCEPT {}

            void deallocate (T* p, size_t num) {
                secureZeroMemory(reinterpret_cast<char*>(p), num);
                std::allocator<T>::deallocate(p, num);
            }

            SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(SafeAllocator)
    };
}
