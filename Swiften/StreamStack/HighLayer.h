/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_HIGHLAYER_H
#define SWIFTEN_HIGHLAYER_H

#include "Swiften/Base/boost_signalslib.h"

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class HighLayer {
		public:
			virtual ~HighLayer();

			virtual void handleDataRead(const ByteArray& data) = 0;

			boost::signal<void (const ByteArray&)> onWriteData;
	};
}

#endif
