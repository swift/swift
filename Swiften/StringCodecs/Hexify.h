#pragma once

namespace Swift {
	class String;
	class ByteArray;

	class Hexify {
		public:
			static String hexify(const ByteArray& data);
	};
}
