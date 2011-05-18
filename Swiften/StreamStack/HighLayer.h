/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class LowLayer;

	class HighLayer {
			friend class StreamStack;

		public:
			HighLayer();
			virtual ~HighLayer();

			virtual void handleDataRead(const SafeByteArray& data) = 0;
	
		protected:
			LowLayer* getChildLayer() {
				return childLayer;
			}

			void setChildLayer(LowLayer* childLayer) {
				this->childLayer = childLayer;
			}

			void writeDataToChildLayer(const SafeByteArray& data);

		private:
			LowLayer* childLayer;
	};
}
