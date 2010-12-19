/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class HighLayer;

	class LowLayer {
			friend class StreamStack;

		public:
			LowLayer();
			virtual ~LowLayer();

			virtual void writeData(const ByteArray& data) = 0;
	
		protected:
			HighLayer* getParentLayer() {
				return parentLayer;
			}

			void setParentLayer(HighLayer* parentLayer) {
				this->parentLayer = parentLayer;
			}

			void writeDataToParentLayer(const ByteArray& data);

		private:
			HighLayer* parentLayer;
	};
}
