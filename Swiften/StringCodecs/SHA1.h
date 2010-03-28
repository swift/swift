#pragma once

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class SHA1 {
		public:
			static ByteArray getHash(const ByteArray& data);
	};
}
