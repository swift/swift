/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/format.hpp>
#include <iostream>

namespace Swift {
	inline boost::format format(const std::string& s) {
		using namespace boost::io;
		try {
			boost::format fmter(s);
			fmter.exceptions(no_error_bits);
			return fmter;
		}
		catch (...) {
			std::cerr << "Error: Invalid translation: " << s << std::endl;
			throw;
		}
	}
}
