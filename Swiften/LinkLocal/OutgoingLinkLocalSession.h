/*
 * Copyright (c) 2010-2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

#include <Swiften/Session/Session.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class ConnectionFactory;
	class XMLParserFactory;	
	class ToplevelElement;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;

	class OutgoingLinkLocalSession : public Session {
		public:
			OutgoingLinkLocalSession(
					const JID& localJID,
					const JID& remoteJID,
					boost::shared_ptr<Connection> connection,
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers,
					XMLParserFactory* xmlParserFactory);

			void queueElement(boost::shared_ptr<ToplevelElement> element);

		private:
			void handleSessionStarted();
			void handleElement(boost::shared_ptr<ToplevelElement>);
			void handleStreamStart(const ProtocolHeader&);

		private:
			std::vector<boost::shared_ptr<ToplevelElement> > queuedElements_;
	};
}
