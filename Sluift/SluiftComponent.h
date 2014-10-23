/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>
#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <Swiften/Client/ClientOptions.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Roster/XMPPRosterItem.h>
#include <Swiften/Component/ComponentError.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Component/Component.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Sluift/Watchdog.h>
#include <Sluift/Response.h>

namespace Swift {
	struct SluiftGlobals;
	class ComponentXMLTracer;
	class Component;
	class Stanza;
	class Payload;
	class ErrorPayload;
	class JID;

	class SluiftComponent {
		public:
			struct Event {
				enum Type {
					MessageType,
					PresenceType
				};

				Event(boost::shared_ptr<Message> stanza) : type(MessageType), stanza(stanza) {}
				Event(boost::shared_ptr<Presence> stanza) : type(PresenceType), stanza(stanza) {}

				Type type;

				// Message & Presence
				boost::shared_ptr<Stanza> stanza;
			};

			SluiftComponent(
					const JID& jid, 
					const std::string& password, 
					NetworkFactories* networkFactories, 
					SimpleEventLoop* eventLoop);
			~SluiftComponent();

			Component* getComponent() {
				return component;
			}

			void connect(const std::string& host, int port);
			void waitConnected(int timeout);
			bool isConnected() const;
			void setTraceEnabled(bool b);

			template<typename REQUEST_TYPE>
			Sluift::Response sendRequest(REQUEST_TYPE request, int timeout) {
				boost::signals::scoped_connection c = request->onResponse.connect(
						boost::bind(&SluiftComponent::handleRequestResponse, this, _1, _2));
				return doSendRequest(request, timeout);
			}

			template<typename REQUEST_TYPE>
			Sluift::Response sendVoidRequest(REQUEST_TYPE request, int timeout) {
				boost::signals::scoped_connection c = request->onResponse.connect(
						boost::bind(&SluiftComponent::handleRequestResponse, this, boost::shared_ptr<Payload>(), _1));
				return doSendRequest(request, timeout);
			}

			void disconnect();
			void setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os);
			boost::optional<SluiftComponent::Event> getNextEvent(int timeout, 
					boost::function<bool (const Event&)> condition = 0);

		private:
			Sluift::Response doSendRequest(boost::shared_ptr<Request> request, int timeout);

			void handleIncomingMessage(boost::shared_ptr<Message> stanza);
			void handleIncomingPresence(boost::shared_ptr<Presence> stanza);
			void handleRequestResponse(boost::shared_ptr<Payload> response, boost::shared_ptr<ErrorPayload> error);
			void handleError(const boost::optional<ComponentError>& error);
		
		private:
			NetworkFactories* networkFactories;
			SimpleEventLoop* eventLoop;
			Component* component;
			ComponentXMLTracer* tracer;
			std::deque<Event> pendingEvents;
			boost::optional<ComponentError> disconnectedError;
			bool requestResponseReceived;
			boost::shared_ptr<Payload> requestResponse;
			boost::shared_ptr<ErrorPayload> requestError;
	};
}
