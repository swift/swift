/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_OpenSSLLayer_H
#define SWIFTEN_OpenSSLLayer_H

#include <boost/noncopyable.hpp>
#include "Swiften/Base/boost_signalslib.h"

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
