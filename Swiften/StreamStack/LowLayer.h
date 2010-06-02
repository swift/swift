/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_LOWLAYER_H
#define SWIFTEN_LOWLAYER_H

#include "Swiften/Base/boost_signalslib.h"

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class LowLayer {
		public:
			virtual ~LowLayer();

			virtual void writeData(const ByteArray& data) = 0;

			boost::signal<void (const ByteArray&)> onDataRead;
	};
}

#endif
