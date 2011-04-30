/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Compress/ZLibCodecompressor.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class ZLibCompressor : public ZLibCodecompressor {
		public:
			ZLibCompressor();

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
