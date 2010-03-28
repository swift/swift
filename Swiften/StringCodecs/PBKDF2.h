#pragma once

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class PBKDF2 {
		public:
			static ByteArray encode(const ByteArray& password, const ByteArray& salt, int iterations);
	};
}
