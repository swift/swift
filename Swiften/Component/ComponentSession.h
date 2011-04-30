/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <Swiften/JID/JID.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/Error.h>
#include <string>
#include <Swiften/Elements/Element.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Session/SessionStream.h>

namespace Swift {
	class ComponentAuthenticator;

	class ComponentSession : public boost::enable_shared_from_this<ComponentSession> {
		public:
			enum State {
				Initial,
				WaitingForStreamStart,
				Authenticating,
				Initialized,
				Finishing,
				Finished
			};

			struct Error : public Swift::Error {
				enum Type {
					AuthenticationFailedError,
					UnexpectedElementError,
				} type;
				Error(Type type) : type(type) {}
			};

			~ComponentSession();

			static boost::shared_ptr<ComponentSession> create(const JID& jid, const std::string& secret, boost::shared_ptr<SessionStream> stream) {
				return boost::shared_ptr<ComponentSession>(new ComponentSession(jid, secret, stream));
			}

			State getState() const {
				return state;
			}

			void start();
			void finish();

			void sendStanza(boost::shared_ptr<Stanza>);

		public:
			boost::signal<void ()> onInitialized;
			boost::signal<void (boost::shared_ptr<Swift::Error>)> onFinished;
			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaReceived;
		
		private:
			ComponentSession(const JID& jid, const std::string& secret, boost::shared_ptr<SessionStream>);

			void finishSession(Error::Type error);
			void finishSession(boost::shared_ptr<Swift::Error> error);

			void sendStreamHeader();

			void handleElement(boost::shared_ptr<Element>);
			void handleStreamStart(const ProtocolHeader&);
			void handleStreamClosed(boost::shared_ptr<Swift::Error>);

			bool checkState(State);

		private:
			JID jid;
			std::string secret;
			boost::shared_ptr<SessionStream> stream;
			boost::shared_ptr<Swift::Error> error;
			State state;
	};
}
