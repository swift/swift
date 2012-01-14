/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

// Conversion utilities
#define NS2STDSTRING(a) (a == nil ? std::string() : std::string([a cStringUsingEncoding:NSUTF8StringEncoding]))
#define STD2NSSTRING(a) [NSString stringWithCString:a.c_str() encoding:NSUTF8StringEncoding]

// Intrusive pointer for NSObjects
namespace boost {	
	inline void intrusive_ptr_add_ref(NSObject* object) {
		[object retain];
	}
	
	inline void intrusive_ptr_release(NSObject* object) {
		[object release];
	}
}

// Including intrusive_ptr after ref/release methods to avoid compilation 
// errors with CLang

#include <boost/intrusive_ptr.hpp>
