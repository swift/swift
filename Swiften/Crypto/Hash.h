/*
 * Copyright (c) 2013  Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class Hash {
		public:
			virtual ~Hash();

			virtual Hash& update(const ByteArray& data) = 0;
			virtual Hash& update(const SafeByteArray& data) = 0;

			virtual std::vector<unsigned char> getHash() = 0;
	};
}
