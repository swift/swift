/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <zlib.h>

#include <Swiften/Base/ByteArray.h>

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
