#ifndef SWIFTEN_OpenSSLLayerFactory_H
#define SWIFTEN_OpenSSLLayerFactory_H

#include "Swiften/StreamStack/TLSLayerFactory.h"

namespace Swift {
	class PlatformTLSLayerFactory : public TLSLayerFactory {
		public:
			PlatformTLSLayerFactory();

			bool canCreate() const;
			virtual TLSLayer* createTLSLayer();
	};
}

#endif
