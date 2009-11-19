#pragma once

namespace Swift {
	class ByteArray;

	class HMACSHA1 {
		public:
			static ByteArray getResult(const ByteArray& key, const ByteArray& data);
	};
}
