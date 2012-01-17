/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/SafeString.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Network/BOSHConnectionPool.h>
#include <Swiften/Session/SessionStream.h>
#include <Swiften/Elements/StreamType.h>
#include <Swiften/EventLoop/EventOwner.h>

namespace Swift {
	class TimerFactory;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class StreamStack;
	class XMPPLayer;
	class ConnectionLayer;
	class CompressionLayer;
	class XMLParserFactory;
	class TLSContextFactory;
	class EventLoop;

	class BOSHSessionStream : public SessionStream, public EventOwner, public boost::enable_shared_from_this<BOSHSessionStream> {
		public:
			BOSHSessionStream(
					const URL& boshURL,
					PayloadParserFactoryCollection* payloadParserFactories,
					PayloadSerializerCollection* payloadSerializers,
					ConnectionFactory* connectionFactory,
					TLSContextFactory* tlsContextFactory,
					TimerFactory* whitespacePingLayerFactory,
					XMLParserFactory* xmlParserFactory,
					EventLoop* eventLoop,
					DomainNameResolver* resolver,
					const std::string& to,
					const URL& boshHTTPConnectProxyURL,
					const SafeString& boshHTTPConnectProxyAuthID,
					const SafeString& boshHTTPConnectProxyAuthPassword
			);
			~BOSHSessionStream();

			virtual void close();
			virtual bool isOpen();

			virtual void writeHeader(const ProtocolHeader& header);
			virtual void writeElement(boost::shared_ptr<Element>);
			virtual void writeFooter();
			virtual void writeData(const std::string& data);

			virtual bool supportsZLibCompression();
			virtual void addZLibCompression();

			virtual bool supportsTLSEncryption();
			virtual void addTLSEncryption();
			virtual bool isTLSEncrypted();
			virtual Certificate::ref getPeerCertificate() const;
			virtual boost::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const;
			virtual ByteArray getTLSFinishMessage() const;

			virtual void setWhitespacePingEnabled(bool);

			virtual void resetXMPPParser();

		private:
			void handleXMPPError();
			void handleStreamStartReceived(const ProtocolHeader&);
			void handleElementReceived(boost::shared_ptr<Element>);
			void handlePoolXMPPDataRead(const SafeByteArray& data);
			void handleXMPPLayerDataWritten(const SafeByteArray& data);
			void handlePoolSessionStarted();
			void handlePoolBOSHDataRead(const SafeByteArray& data);
			void handlePoolBOSHDataWritten(const SafeByteArray& data);
			void handlePoolSessionTerminated(BOSHError::ref condition);

		private:
			void fakeStreamHeaderReceipt();
			void fakeStreamFooterReceipt(BOSHError::ref error);

		private:
			BOSHConnectionPool* connectionPool;
			bool available;
			PayloadParserFactoryCollection* payloadParserFactories;
			PayloadSerializerCollection* payloadSerializers;
			TLSContextFactory* tlsContextFactory;
			TimerFactory* timerFactory;
			XMLParserFactory* xmlParserFactory;
			XMPPLayer* xmppLayer;
			ProtocolHeader streamHeader;
			EventLoop* eventLoop;
			bool firstHeader;
	};

}
