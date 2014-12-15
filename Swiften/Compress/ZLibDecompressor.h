/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Compress/ZLibCodecompressor.h>
#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API ZLibDecompressor : public ZLibCodecompressor {
		public:
			ZLibDecompressor();
			~ZLibDecompressor();

			virtual int processZStream();
	};
}
