/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateVerificationError.h>
#include <Swiften/TLS/CertificateWithKey.h>

namespace Swift {
    class SWIFTEN_API SessionStream {
        public:
            class SWIFTEN_API SessionStreamError : public Swift::Error {
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
            virtual void writeElement(std::shared_ptr<ToplevelElement>) = 0;
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
            virtual std::vector<Certificate::ref> getPeerCertificateChain() const = 0;
            virtual std::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const = 0;

            virtual ByteArray getTLSFinishMessage() const = 0;

            boost::signals2::signal<void (const ProtocolHeader&)> onStreamStartReceived;
            boost::signals2::signal<void ()> onStreamEndReceived;
            boost::signals2::signal<void (std::shared_ptr<ToplevelElement>)> onElementReceived;
            boost::signals2::signal<void (std::shared_ptr<Error>)> onClosed;
            boost::signals2::signal<void ()> onTLSEncrypted;
            boost::signals2::signal<void (const SafeByteArray&)> onDataRead;
            boost::signals2::signal<void (const SafeByteArray&)> onDataWritten;

        protected:
            CertificateWithKey::ref getTLSCertificate() const {
                return certificate;
            }

        private:
            CertificateWithKey::ref certificate;
    };
}
