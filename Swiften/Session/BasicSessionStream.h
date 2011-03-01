/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Network/Connection.h"
#include "Swiften/Session/SessionStream.h"
#include "Swiften/Elements/StreamType.h"

namespace Swift {
	class TLSContextFactory;
	class TLSLayer;
	class TimerFactory;
	class WhitespacePingLayer;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class StreamStack;
	class XMPPLayer;
	class ConnectionLayer;
	class CompressionLayer;

	class BasicSessionStream : public SessionStream {
		public:
			BasicSessionStream(
				StreamType streamType,
				boost::shared_ptr<Connection> connection,
				PayloadParserFactoryCollection* payloadParserFactories, 
				PayloadSerializerCollection* payloadSerializers,
				TLSContextFactory* tlsContextFactory,
				TimerFactory* whitespacePingLayerFactory
			);
			~BasicSessionStream();

			virtual void close();
			virtual bool isOpen();

			virtual void writeHeader(const ProtocolHeader& header);
			virtual void writeElement(boost::shared_ptr<Element>);
			virtual void writeFooter();
			virtual void writeData(const std::string& data);

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
			void handleConnectionFinished(const boost::optional<Connection::Error>& error);
			void handleXMPPError();
			void handleTLSConnected();
			void handleTLSError();
			void handleStreamStartReceived(const ProtocolHeader&);
			void handleElementReceived(boost::shared_ptr<Element>);
			void handleDataRead(const ByteArray& data);
			void handleDataWritten(const ByteArray& data);

		private:
			bool available;
			boost::shared_ptr<Connection> connection;
			PayloadParserFactoryCollection* payloadParserFactories;
			PayloadSerializerCollection* payloadSerializers;
			TLSContextFactory* tlsContextFactory;
			TimerFactory* timerFactory;
			StreamType streamType;
			XMPPLayer* xmppLayer;
			ConnectionLayer* connectionLayer;
			CompressionLayer* compressionLayer;
			TLSLayer* tlsLayer;
			WhitespacePingLayer* whitespacePingLayer;
			StreamStack* streamStack;
	};

}
