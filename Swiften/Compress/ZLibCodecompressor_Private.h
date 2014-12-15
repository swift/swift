/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Compress/ZLibCodecompressor.h>

#include <zlib.h>

namespace Swift {
	struct ZLibCodecompressor::Private {
		z_stream stream;
	};
}
