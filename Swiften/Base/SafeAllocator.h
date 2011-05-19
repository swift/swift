/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <algorithm>

namespace Swift {
	template<typename T>
	class SafeAllocator : public std::allocator<T> {
		public:
			template <class U> struct rebind {
				typedef SafeAllocator<U> other;
			};

			SafeAllocator() throw() {}
			SafeAllocator(const SafeAllocator&) throw() : std::allocator<T>() {}
			template <class U> SafeAllocator(const SafeAllocator<U>&) throw() {}
			~SafeAllocator() throw() {}

			void deallocate (T* p, size_t num) {
				std::fill(reinterpret_cast<char*>(p), reinterpret_cast<char*>(p + num), 0);
				std::allocator<T>::deallocate(p, num);
			}
	};
};
