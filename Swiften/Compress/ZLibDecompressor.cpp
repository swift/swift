/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Compress/ZLibDecompressor.h>

#include <cassert>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

ZLibDecompressor::ZLibDecompressor() {
	int result = inflateInit(&stream_);
	assert(result == Z_OK);
	(void) result;
}

}
