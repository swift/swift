#ifndef SWIFTEN_OpenSSLLayer_H
#define SWIFTEN_OpenSSLLayer_H

#include <boost/noncopyable.hpp>
#include <boost/signal.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/StreamStack/TLSLayer.h"
#include "Swiften/TLS/OpenSSL/OpenSSLContext.h"

namespace Swift {
	class OpenSSLLayer : public TLSLayer, boost::noncopyable {
		public:
			OpenSSLLayer();

			virtual void connect();
			virtual bool setClientCertificate(const PKCS12Certificate&);

			virtual void writeData(const ByteArray& data);
			virtual void handleDataRead(const ByteArray& data);

		private:
			OpenSSLContext context_;
	};
}

#endif
