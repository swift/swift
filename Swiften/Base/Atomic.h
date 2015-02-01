/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

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
			boost::lock_guard<boost::mutex> lock(valueMutex_);
			value_ = newValue;
			return *this;
		}

		/**
		 * Synchronized read access.
		 */
		operator ValueType() {
			boost::lock_guard<boost::mutex> lock(valueMutex_);
			return value_;
		}

	private:
		boost::mutex valueMutex_;
		ValueType value_;
};

}
