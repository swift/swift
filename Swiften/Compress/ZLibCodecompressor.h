/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class ZLibCodecompressor {
		public:
			ZLibCodecompressor();
			virtual ~ZLibCodecompressor();

			SafeByteArray process(const SafeByteArray& data);
			virtual int processZStream() = 0;

		protected:
			struct Private;
			boost::shared_ptr<Private> p;
	};
}
