#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Network/Connection.h"
#include "Swiften/Session/SessionStream.h"

namespace Swift {
	class TLSLayerFactory;
	class TLSLayer;
	class WhitespacePingLayer;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class StreamStack;
	class XMPPLayer;
	class ConnectionLayer;

	class BasicSessionStream : 
			public SessionStream, 
			public boost::enable_shared_from_this<BasicSessionStream> {
		public:
			BasicSessionStream(
				boost::shared_ptr<Connection> connection,
				PayloadParserFactoryCollection* payloadParserFactories, 
				PayloadSerializerCollection* payloadSerializers,
				TLSLayerFactory* tlsLayerFactory
			);
			~BasicSessionStream();

			void initialize();

			virtual bool isAvailable();

			virtual void writeHeader(const ProtocolHeader& header);
			virtual void writeElement(boost::shared_ptr<Element>);
			virtual void writeFooter();

			virtual bool supportsTLSEncryption();
			virtual void addTLSEncryption();

			virtual void setWhitespacePingEnabled(bool);

			virtual void resetXMPPParser();

		private:
			void handleConnectionError(const boost::optional<Connection::Error>& error);
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
			TLSLayerFactory* tlsLayerFactory;
			boost::shared_ptr<XMPPLayer> xmppLayer;
			boost::shared_ptr<ConnectionLayer> connectionLayer;
			StreamStack* streamStack;
			boost::shared_ptr<TLSLayer> tlsLayer;
			boost::shared_ptr<WhitespacePingLayer> whitespacePingLayer;
	};
}
