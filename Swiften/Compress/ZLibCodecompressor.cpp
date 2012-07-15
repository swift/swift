/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Compress/ZLibCodecompressor.h>

#include <cassert>
#include <string.h>
#include <zlib.h>

#include <Swiften/Compress/ZLibException.h>
#include <Swiften/Compress/ZLibCodecompressor_Private.h>

namespace Swift {

static const int CHUNK_SIZE = 1024; // If you change this, also change the unittest


ZLibCodecompressor::ZLibCodecompressor() : p(boost::make_shared<Private>()) {
	memset(&p->stream, 0, sizeof(z_stream));
	p->stream.zalloc = Z_NULL;
	p->stream.zfree = Z_NULL;
	p->stream.opaque = Z_NULL;
}

ZLibCodecompressor::~ZLibCodecompressor() {
}

SafeByteArray ZLibCodecompressor::process(const SafeByteArray& input) {
	SafeByteArray output;
	p->stream.avail_in = input.size();
	p->stream.next_in = reinterpret_cast<Bytef*>(const_cast<unsigned char*>(vecptr(input)));
	int outputPosition = 0;
	do {
		output.resize(outputPosition + CHUNK_SIZE);
		p->stream.avail_out = CHUNK_SIZE;
		p->stream.next_out = reinterpret_cast<Bytef*>(vecptr(output) + outputPosition);
		int result = processZStream();
		if (result != Z_OK && result != Z_BUF_ERROR) {
			throw ZLibException(/* p->stream.msg */);
		}
		outputPosition += CHUNK_SIZE;
	}
	while (p->stream.avail_out == 0);
	if (p->stream.avail_in != 0) {
		throw ZLibException();
	}
	output.resize(outputPosition - p->stream.avail_out);
	return output;
}

}
