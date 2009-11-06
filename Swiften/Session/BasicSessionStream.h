#pragma once

#include <boost/shared_ptr.hpp>

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
      public boost::BOOST_SIGNALS_NAMESPACE::trackable {
    public:
      BasicSessionStream(
		    boost::shared_ptr<Connection> connection,
        PayloadParserFactoryCollection* payloadParserFactories, 
        PayloadSerializerCollection* payloadSerializers,
		    TLSLayerFactory* tlsLayerFactory
      );
      ~BasicSessionStream();

			virtual void writeHeader(const ProtocolHeader& header);
			virtual void writeElement(boost::shared_ptr<Element>);

			virtual bool supportsTLSEncryption();
			virtual void addTLSEncryption();

			virtual void addWhitespacePing();

			virtual void resetXMPPParser();

    private:
      void handleXMPPError();
      void handleTLSError();

    private:
			boost::shared_ptr<XMPPLayer> xmppLayer;
			boost::shared_ptr<ConnectionLayer> connectionLayer;
			StreamStack* streamStack;
      TLSLayerFactory* tlsLayerFactory;
      boost::shared_ptr<TLSLayer> tlsLayer;
      boost::shared_ptr<WhitespacePingLayer> whitespacePingLayer;
  };
}
