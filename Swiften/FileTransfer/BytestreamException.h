/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <stdexcept>

namespace Swift {
	class BytestreamException : public std::exception {
		public:
			BytestreamException() {
			}
	};
}
