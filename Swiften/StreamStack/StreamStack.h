/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <vector>

#include <Swiften/Elements/Stanza.h>

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
				for (size_t i = 0; i < layers_.size(); ++i) {
					T* layer = dynamic_cast<T*>(layers_[i]);
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
