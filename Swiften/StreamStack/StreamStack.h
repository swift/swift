/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "Swiften/Base/boost_bsignals.h"
#include <vector>

#include "Swiften/Elements/Stanza.h"
#include "Swiften/Base/foreach.h"

namespace Swift {
	class XMPPLayer;
	class LowLayer;
	class StreamLayer;

	class StreamStack {
		public:
			StreamStack(XMPPLayer* xmppLayer, LowLayer* physicalLayer);
			~StreamStack();

			void addLayer(StreamLayer*);

			XMPPLayer* getXMPPLayer() const {
				return xmppLayer_;
			}

			template<typename T> T* getLayer() {
				foreach(StreamLayer* streamLayer, layers_) {
					T* layer = dynamic_cast<T*>(streamLayer);
					if (layer) {
						return layer;
					}
				}
				return NULL;
			}

		private:
			XMPPLayer* xmppLayer_;
			LowLayer* physicalLayer_;
			std::vector<StreamLayer*> layers_;
	};
}
