#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class Element;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class StreamStack;
	class UserRegistry;
	class XMPPLayer;
	class IncomingConnectionLayer;
	class IncomingConnection;
	class ByteArray;

	class ServerFromClientSession {
		public:
			ServerFromClientSession(
					const String& id,
					boost::shared_ptr<IncomingConnection> connection, 
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers,
					UserRegistry* userRegistry);
			~ServerFromClientSession();

			boost::signal<void (boost::shared_ptr<Element>)> onElementReceived;
			boost::signal<void ()> onSessionFinished;
			boost::signal<void (const ByteArray&)> onDataWritten;
			boost::signal<void (const ByteArray&)> onDataRead;

		private:
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const String& domain);

		private:
			String id_;
			boost::shared_ptr<IncomingConnection> connection_;
			PayloadParserFactoryCollection* payloadParserFactories_;
			PayloadSerializerCollection* payloadSerializers_;
			UserRegistry* userRegistry_;
			bool authenticated_;
			bool initialized_;
			IncomingConnectionLayer* connectionLayer_;
			StreamStack* streamStack_;
			XMPPLayer* xmppLayer_;
			String domain_;
			String user_;
			JID jid_;
	};
}
