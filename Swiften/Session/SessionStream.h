/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Elements/Element.h>
#include <Swiften/Base/Error.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateVerificationError.h>

namespace Swift {
	class SessionStream {
		public:
			class SessionStreamError : public Swift::Error {
				public:
					enum Type {
						ParseError,
						TLSError,
						InvalidTLSCertificateError,
						ConnectionReadError,
						ConnectionWriteError
					};

					SessionStreamError(Type type) : type(type) {}

					Type type;
			};

			SessionStream(): certificate() {}

			virtual ~SessionStream();

			virtual void close() = 0;
			virtual bool isOpen() = 0;

			virtual void writeHeader(const ProtocolHeader& header) = 0;
			virtual void writeFooter() = 0;
			virtual void writeElement(boost::shared_ptr<Element>) = 0;
			virtual void writeData(const std::string& data) = 0;

			virtual bool supportsZLibCompression() = 0;
			virtual void addZLibCompression() = 0;

			virtual bool supportsTLSEncryption() = 0;
			virtual void addTLSEncryption() = 0;
			virtual bool isTLSEncrypted() = 0;
			virtual void setWhitespacePingEnabled(bool enabled) = 0;

			virtual void resetXMPPParser() = 0;

			void setTLSCertificate(CertificateWithKey::ref cert) {
				certificate = cert;
			}

			virtual bool hasTLSCertificate() {
				return certificate && !certificate->isNull();
			}

			virtual Certificate::ref getPeerCertificate() const = 0;
			virtual boost::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const = 0;

			virtual ByteArray getTLSFinishMessage() const = 0;

			boost::signal<void (const ProtocolHeader&)> onStreamStartReceived;
			boost::signal<void (boost::shared_ptr<Element>)> onElementReceived;
			boost::signal<void (boost::shared_ptr<Error>)> onClosed;
			boost::signal<void ()> onTLSEncrypted;
			boost::signal<void (const SafeByteArray&)> onDataRead;
			boost::signal<void (const SafeByteArray&)> onDataWritten;

		protected:
			CertificateWithKey::ref getTLSCertificate() const {
				return certificate;
			}

		private:
			CertificateWithKey::ref certificate;
	};
}
