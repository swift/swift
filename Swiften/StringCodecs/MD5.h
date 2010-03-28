#pragma once

namespace Swift {
	class ByteArray;

	class MD5 {
		public:
			static ByteArray getHash(const ByteArray& data);
	};
}
