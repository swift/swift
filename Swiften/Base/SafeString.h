/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API SafeString {
		public:
			SafeString(const SafeByteArray& data) : data(data) {
			}

			SafeString(const std::string& s) {
				data = createSafeByteArray(s);
			}

			SafeString(const char* s) {
				data = createSafeByteArray(s);
			}

			operator SafeByteArray () const {
				return data;
			}

		private:
			SafeByteArray data;
	};
}
