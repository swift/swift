#ifndef SWIFTEN_SHA1_H
#define SWIFTEN_SHA1_H

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class SHA1 {
		public:
			static ByteArray getBinaryHash(const ByteArray& data);
			static String getHexHash(const ByteArray& data);
		};
}

#endif
