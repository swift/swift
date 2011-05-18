/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Compress/ZLibCodecompressor.h>

#include <cassert>
#include <string.h>

#include <Swiften/Compress/ZLibException.h>

namespace Swift {

static const int CHUNK_SIZE = 1024; // If you change this, also change the unittest

ZLibCodecompressor::ZLibCodecompressor() {
	memset(&stream_, 0, sizeof(z_stream));
	stream_.zalloc = Z_NULL;
	stream_.zfree = Z_NULL;
	stream_.opaque = Z_NULL;
}

ZLibCodecompressor::~ZLibCodecompressor() {
}

SafeByteArray ZLibCodecompressor::process(const SafeByteArray& input) {
	SafeByteArray output;
	stream_.avail_in = input.size();
	stream_.next_in = reinterpret_cast<Bytef*>(const_cast<unsigned char*>(vecptr(input)));
	int outputPosition = 0;
	do {
		output.resize(outputPosition + CHUNK_SIZE);
		stream_.avail_out = CHUNK_SIZE;
		stream_.next_out = reinterpret_cast<Bytef*>(vecptr(output) + outputPosition);
		int result = processZStream();
		if (result != Z_OK && result != Z_BUF_ERROR) {
			throw ZLibException(/* stream_.msg */);
		}
		outputPosition += CHUNK_SIZE;
	}
	while (stream_.avail_out == 0);
	if (stream_.avail_in != 0) {
		throw ZLibException();
	}
	output.resize(outputPosition - stream_.avail_out);
	return output;
}

}
