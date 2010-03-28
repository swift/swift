#ifndef SWIFTEN_ZLibCodecompressor_H
#define SWIFTEN_ZLibCodecompressor_H

#include <zlib.h>

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class ZLibCodecompressor {
		public:
			ZLibCodecompressor();
			virtual ~ZLibCodecompressor();

			ByteArray process(const ByteArray& data);
			virtual int processZStream() = 0;

		protected:
			z_stream stream_;
	};
}

#endif
