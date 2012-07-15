/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Compress/ZLibCodecompressor.h>

#include <zlib.h>

namespace Swift {
	struct ZLibCodecompressor::Private {
		z_stream stream;
	};
}
