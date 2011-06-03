/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class SafeString {
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
