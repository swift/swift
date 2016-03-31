/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <zlib.h>

#include <Swiften/Compress/ZLibCodecompressor.h>

namespace Swift {
    struct ZLibCodecompressor::Private {
        z_stream stream;
    };
}
