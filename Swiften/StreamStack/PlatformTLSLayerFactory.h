#pragma once

#include "Swiften/StreamStack/TLSLayerFactory.h"

namespace Swift {
	class PlatformTLSLayerFactory : public TLSLayerFactory {
		public:
			PlatformTLSLayerFactory();

			bool canCreate() const;
			virtual boost::shared_ptr<TLSLayer> createTLSLayer();
	};
}
