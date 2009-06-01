#ifndef SWIFTEN_STRINGCODECS_BASE64_H
#define SWIFTEN_STRINGCODECS_BASE64_H

#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class Base64
	{
		public:
			static String encode(const ByteArray& s);
			static ByteArray decode(const String &s);
	};
}

#endif
