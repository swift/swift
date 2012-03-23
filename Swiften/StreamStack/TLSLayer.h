/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/StreamStack/StreamLayer.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/TLS/CertificateVerificationError.h>
#include <Swiften/TLS/TLSError.h>

namespace Swift {
	class TLSContext;
	class TLSContextFactory;

	class TLSLayer : public StreamLayer {
		public:
			TLSLayer(TLSContextFactory*);
			~TLSLayer();

			void connect();
			bool setClientCertificate(CertificateWithKey::ref cert);

			Certificate::ref getPeerCertificate() const;
			boost::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const;

			void writeData(const SafeByteArray& data);
			void handleDataRead(const SafeByteArray& data);

			TLSContext* getContext() const {
				return context;
			}

		public:
			boost::signal<void (boost::shared_ptr<TLSError>)> onError;
			boost::signal<void ()> onConnected;

		private:
			TLSContext* context;
	};
}
