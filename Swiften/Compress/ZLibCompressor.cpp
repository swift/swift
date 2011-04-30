/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Compress/ZLibCompressor.h>

#include <cassert>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

ZLibCompressor::ZLibCompressor() {
	int result = deflateInit(&stream_, COMPRESSION_LEVEL);
	assert(result == Z_OK);
	(void) result;
}

}
