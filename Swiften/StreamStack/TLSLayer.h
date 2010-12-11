/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/boost_bsignals.h"

#include "Swiften/Base/ByteArray.h"
#include "Swiften/StreamStack/StreamLayer.h"
#include "Swiften/TLS/Certificate.h"
#include "Swiften/TLS/CertificateVerificationError.h"

namespace Swift {
	class TLSContext;
	class TLSContextFactory;
	class PKCS12Certificate;

	class TLSLayer : public StreamLayer {
		public:
			TLSLayer(TLSContextFactory*);
			~TLSLayer();

			void connect();
			bool setClientCertificate(const PKCS12Certificate&);

			Certificate::ref getPeerCertificate() const;
			boost::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const;

			void writeData(const ByteArray& data);
			void handleDataRead(const ByteArray& data);

			TLSContext* getContext() const {
				return context;
			}

		public:
			boost::signal<void ()> onError;
			boost::signal<void ()> onConnected;

		private:
			TLSContext* context;
	};
}
