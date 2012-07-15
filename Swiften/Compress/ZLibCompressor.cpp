/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Compress/ZLibCompressor.h>

#include <cassert>
#include <zlib.h>

#include <Swiften/Compress/ZLibCodecompressor_Private.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

ZLibCompressor::ZLibCompressor() {
	int result = deflateInit(&p->stream, COMPRESSION_LEVEL);
	assert(result == Z_OK);
	(void) result;
}

ZLibCompressor::~ZLibCompressor() {
	deflateEnd(&p->stream);
}

int ZLibCompressor::processZStream() {
	return deflate(&p->stream, Z_SYNC_FLUSH);
}

}
