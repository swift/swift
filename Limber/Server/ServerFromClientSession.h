/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Session/Session.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class ProtocolHeader;
	class ToplevelElement;
	class Stanza;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class StreamStack;
	class UserRegistry;
	class XMPPLayer;
	class ConnectionLayer;
	class Connection;
	class XMLParserFactory;

	class ServerFromClientSession : public Session {
		public:
			ServerFromClientSession(
					const std::string& id,
					boost::shared_ptr<Connection> connection, 
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers,
					XMLParserFactory* xmlParserFactory,
					UserRegistry* userRegistry);

			boost::signal<void ()> onSessionStarted;
			void setAllowSASLEXTERNAL();

		private:
			void handleElement(boost::shared_ptr<ToplevelElement>);
			void handleStreamStart(const ProtocolHeader& header);

			void setInitialized();
			bool isInitialized() const { 
				return initialized; 
			}

		private:
			std::string id_;
			UserRegistry* userRegistry_;
			bool authenticated_;
			bool initialized;
			bool allowSASLEXTERNAL;
			std::string user_;
	};
}
