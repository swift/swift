/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Compress/ZLibDecompressor.h>

#include <zlib.h>
#include <cassert>

#include <Swiften/Compress/ZLibCodecompressor_Private.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

ZLibDecompressor::ZLibDecompressor() {
	int result = inflateInit(&p->stream);
	assert(result == Z_OK);
	(void) result;
}

ZLibDecompressor::~ZLibDecompressor() {
	inflateEnd(&p->stream);
}

int ZLibDecompressor::processZStream() {
	return inflate(&p->stream, Z_SYNC_FLUSH);
}

}
