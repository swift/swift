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

			virtual void writeHeader(const ProtocolHeader& header);
			virtual void writeElement(boost::shared_ptr<Element>);

			virtual bool supportsTLSEncryption();
			virtual void addTLSEncryption();

			virtual void addWhitespacePing();

			virtual void resetXMPPParser();

    private:
      void handleXMPPError();
      void handleTLSError();
			void handleStreamStartReceived(const ProtocolHeader&);
			void handleElementReceived(boost::shared_ptr<Element>);

    private:
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
