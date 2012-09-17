/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
