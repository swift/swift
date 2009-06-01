#ifndef SWIFTEN_TLSLayer_H
#define SWIFTEN_TLSLayer_H

#include "Swiften/StreamStack/StreamLayer.h"
#include "Swiften/TLS/PKCS12Certificate.h"

namespace Swift {
	class TLSLayer : public StreamLayer {
		public:
			virtual void connect() = 0;
			virtual bool setClientCertificate(const PKCS12Certificate&) = 0;

		public:
			boost::signal<void ()> onError;
			boost::signal<void ()> onConnected;
	};
}

#endif
