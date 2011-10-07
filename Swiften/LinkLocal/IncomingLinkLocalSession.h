/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Session/Session.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/Connection.h>

namespace Swift {
	class ProtocolHeader;
	class XMLParserFactory;	
	class Element;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;

	class IncomingLinkLocalSession : public Session {
		public:
			IncomingLinkLocalSession(
					const JID& localJID,
					boost::shared_ptr<Connection> connection, 
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers,
					XMLParserFactory* xmlParserFactory);

			boost::signal<void ()> onSessionStarted;

		private:
			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader&);
			void setInitialized();
			bool isInitialized() const { 
				return initialized; 
			}

			bool initialized;
	};
}
