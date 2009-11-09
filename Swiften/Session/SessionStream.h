#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/ProtocolHeader.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/Base/Error.h"
#include "Swiften/TLS/PKCS12Certificate.h"

namespace Swift {
	class SessionStream {
		public:
			class Error : public Swift::Error {
				public:
					Error() {}
			};

			virtual ~SessionStream();

			virtual bool isAvailable() = 0;

			virtual void writeHeader(const ProtocolHeader& header) = 0;
			virtual void writeFooter() = 0;
			virtual void writeElement(boost::shared_ptr<Element>) = 0;

			virtual bool supportsTLSEncryption() = 0;
			virtual void addTLSEncryption() = 0;
			virtual void setWhitespacePingEnabled(bool enabled) = 0;

			virtual void resetXMPPParser() = 0;

			void setTLSCertificate(const PKCS12Certificate& cert) {
				certificate = cert;
			}

			virtual bool hasTLSCertificate() {
				return !certificate.isNull();
			}


			boost::signal<void (const ProtocolHeader&)> onStreamStartReceived;
			boost::signal<void (boost::shared_ptr<Element>)> onElementReceived;
			boost::signal<void (boost::shared_ptr<Error>)> onError;
			boost::signal<void ()> onTLSEncrypted;

		protected:
			const PKCS12Certificate& getTLSCertificate() const {
				return certificate;
			}

		private:
			PKCS12Certificate certificate;
	};
}
