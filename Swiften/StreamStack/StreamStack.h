#ifndef SWIFTEN_STREAMSTACK_H
#define SWIFTEN_STREAMSTACK_H

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
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
				return 0;
			}

		private:
			XMPPLayer* xmppLayer_;
			LowLayer* physicalLayer_;
			std::vector<StreamLayer*> layers_;
			boost::bsignals::connection xmppReadSlotConnection_;
			boost::bsignals::connection xmppWriteSignalConnection_;
	};
}

#endif
