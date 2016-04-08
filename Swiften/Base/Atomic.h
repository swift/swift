/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <mutex>

namespace Swift {

/**
 * This class template provides a read/write access synchronized wrapper for other types.
 */
template <typename ValueType>
class Atomic {
    public:
        Atomic(const ValueType& v) : value_(v) {
        }

        /**
         * Synchronized write access.
         */
        Atomic<ValueType>& operator=(const ValueType& newValue) {
            std::lock_guard<std::mutex> lock(valueMutex_);
            value_ = newValue;
            return *this;
        }

        /**
         * Synchronized read access.
         */
        operator ValueType() {
            std::lock_guard<std::mutex> lock(valueMutex_);
            return value_;
        }

    private:
        std::mutex valueMutex_;
        ValueType value_;
};

}
