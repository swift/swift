/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Compress/ZLibCodecompressor.h>

#include <string.h>

#include <cassert>

#include <boost/numeric/conversion/cast.hpp>

#include <zlib.h>

#include <Swiften/Compress/ZLibCodecompressor_Private.h>
#include <Swiften/Compress/ZLibException.h>

namespace Swift {

static const size_t CHUNK_SIZE = 1024; // If you change this, also change the unittest


ZLibCodecompressor::ZLibCodecompressor() : p(new Private()) {
    memset(&p->stream, 0, sizeof(z_stream));
    p->stream.zalloc = Z_NULL;
    p->stream.zfree = Z_NULL;
    p->stream.opaque = Z_NULL;
}

ZLibCodecompressor::~ZLibCodecompressor() {
}

SafeByteArray ZLibCodecompressor::process(const SafeByteArray& input) {
    SafeByteArray output;
    p->stream.avail_in = static_cast<unsigned int>(input.size());
    p->stream.next_in = reinterpret_cast<Bytef*>(const_cast<unsigned char*>(vecptr(input)));
    size_t outputPosition = 0;
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
