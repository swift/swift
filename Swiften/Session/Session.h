/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>
#include <boost/optional.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Element.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/StreamStack/ConnectionLayer.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class ProtocolHeader;
	class StreamStack;
	class JID;
	class Element;
	class PayloadParserFactoryCollection;
	class PayloadSerializerCollection;
	class XMPPLayer;
	class XMLParserFactory;

	class Session : public boost::enable_shared_from_this<Session> {
		public:
			enum SessionError {
				ConnectionReadError,
				ConnectionWriteError,
				XMLError,
				AuthenticationFailedError,
				NoSupportedAuthMechanismsError,
				UnexpectedElementError,
				ResourceBindError,
				SessionStartError,
				TLSError,
				ClientCertificateLoadError,
				ClientCertificateError
			};

			Session(
					boost::shared_ptr<Connection> connection,
					PayloadParserFactoryCollection* payloadParserFactories, 
					PayloadSerializerCollection* payloadSerializers,
					XMLParserFactory* xmlParserFactory);
			virtual ~Session();

			void startSession();
			void finishSession();

			void sendElement(boost::shared_ptr<Element>);

			const JID& getLocalJID() const {
				return localJID;
			}

			const JID& getRemoteJID() const {
				return remoteJID;
			}

			boost::signal<void (boost::shared_ptr<Element>)> onElementReceived;
			boost::signal<void (const boost::optional<SessionError>&)> onSessionFinished;
			boost::signal<void (const SafeByteArray&)> onDataWritten;
			boost::signal<void (const SafeByteArray&)> onDataRead;

		protected:
			void setRemoteJID(const JID& j) {
				remoteJID = j;
			}

			void setLocalJID(const JID& j) {
				localJID = j;
			}

			void finishSession(const SessionError&);

			virtual void handleSessionStarted() {}
			virtual void handleSessionFinished(const boost::optional<SessionError>&) {}
			virtual void handleElement(boost::shared_ptr<Element>) = 0;
			virtual void handleStreamStart(const ProtocolHeader&) = 0;

			void initializeStreamStack();

			XMPPLayer* getXMPPLayer() const {
				return xmppLayer;
			}

			StreamStack* getStreamStack() const {
				return streamStack;
			}

			void setFinished();

		private:
			void handleDisconnected(const boost::optional<Connection::Error>& error);

		private:
			JID localJID;
			JID remoteJID;
			boost::shared_ptr<Connection> connection;
			PayloadParserFactoryCollection* payloadParserFactories;
			PayloadSerializerCollection* payloadSerializers;
			XMLParserFactory* xmlParserFactory;
			XMPPLayer* xmppLayer;
			ConnectionLayer* connectionLayer;
			StreamStack* streamStack;
			bool finishing;
	};
}
