/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "Swiften/Base/boost_signalslib.h"
#include <vector>

#include "Swiften/Elements/Stanza.h"
#include "Swiften/Base/foreach.h"

namespace Swift {
	class XMPPLayer;
	class LowLayer;
	class StreamLayer;

	class StreamStack {
		public:
			StreamStack(boost::shared_ptr<XMPPLayer> xmppLayer, boost::shared_ptr<LowLayer> physicalLayer);
			~StreamStack();

			void addLayer(boost::shared_ptr<StreamLayer>);

			boost::shared_ptr<XMPPLayer> getXMPPLayer() const {
				return xmppLayer_;
			}

			template<typename T> boost::shared_ptr<T> getLayer() {
				foreach(const boost::shared_ptr<StreamLayer>& streamLayer, layers_) {
					boost::shared_ptr<T> layer = boost::dynamic_pointer_cast<T>(streamLayer);
					if (layer) {
						return layer;
					}
				}
				return boost::shared_ptr<T>();
			}

		private:
			boost::shared_ptr<XMPPLayer> xmppLayer_;
			boost::shared_ptr<LowLayer> physicalLayer_;
			std::vector< boost::shared_ptr<StreamLayer> > layers_;
			boost::bsignals::connection xmppReadSlotConnection_;
			boost::bsignals::connection xmppWriteSignalConnection_;
			std::vector< boost::bsignals::connection > writeSignalConnections_;
	};
}
