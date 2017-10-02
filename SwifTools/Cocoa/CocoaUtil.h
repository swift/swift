/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

// Conversion utilities
namespace {

inline std::string ns2StdString(NSString* _Nullable nsString);
inline std::string ns2StdString(NSString* _Nullable nsString) {
    std::string stdString;
    if (nsString != nil) {
        stdString = std::string([nsString cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    return stdString;
}

inline NSString* _Nonnull std2NSString(const std::string& stdString);
inline NSString* _Nonnull std2NSString(const std::string& stdString) {
    NSString* _Nullable nsString = [NSString stringWithUTF8String:stdString.c_str()];
    if (nsString == nil) {
        nsString = @"";
    }
    // At this point nsString is guaranteed to be not null/nil.
    return static_cast<NSString* _Nonnull>(nsString);
}

}

// Intrusive pointer for NSObjects
namespace boost {
    inline void intrusive_ptr_add_ref(NSObject* _Nonnull object) {
        [object retain];
    }

    inline void intrusive_ptr_release(NSObject* _Nonnull object) {
        [object release];
    }
}

// Including intrusive_ptr after ref/release methods to avoid compilation
// errors with CLang

#include <boost/intrusive_ptr.hpp>
