/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ZLibCompressor_H
#define SWIFTEN_ZLibCompressor_H

#include <cassert>

#include "Swiften/Compress/ZLibCodecompressor.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class ZLibCompressor : public ZLibCodecompressor {
		public:
			ZLibCompressor() {
				int result = deflateInit(&stream_, COMPRESSION_LEVEL);
				assert(result == Z_OK);
				(void) result;
			}

			~ZLibCompressor() {
				deflateEnd(&stream_);
			}

			virtual int processZStream() {
				return deflate(&stream_, Z_SYNC_FLUSH);
			}

		private:
			static const int COMPRESSION_LEVEL = 9;
	};
}

#endif
