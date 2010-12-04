/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>
#include <boost/bind.hpp>

#include <Swiften/Swiften.h>

using namespace Swift;
using namespace boost;

class EchoComponent {
	public:
		EchoComponent(EventLoop* eventLoop, NetworkFactories* networkFactories) {
			component = new Component(eventLoop, networkFactories,
					JID("echo.wonderland.lit"), "EchoSecret");
			component->onConnected.connect(bind(&EchoComponent::handleConnected, this));
			component->onMessageReceived.connect(
					bind(&EchoComponent::handleMessageReceived, this, _1));
			component->onPresenceReceived.connect(
					bind(&EchoComponent::handlePresenceReceived, this, _1));
			tracer = new ComponentXMLTracer(component);
			component->connect("wonderland.lit", 5347);
		}

		~EchoComponent() {
			delete tracer;
			delete component;
		}
	
	private:
		void handlePresenceReceived(Presence::ref presence) {
			// Automatically approve subscription requests
			if (presence->getType() == Presence::Subscribe) {
				Presence::ref response = Presence::create();
				response->setTo(presence->getFrom());
				response->setType(Presence::Subscribed);
				component->sendPresence(response);
			}
		}

		void handleConnected() {
		}

		void handleMessageReceived(Message::ref message) {
			// Echo back the incoming message
			message->setTo(message->getFrom());
			message->setFrom(JID());
			component->sendMessage(message);
		}

	private:
		Component* component;
		ComponentXMLTracer* tracer;
};

int main(int, char**) {
	SimpleEventLoop eventLoop;
	BoostNetworkFactories networkFactories(&eventLoop);

	EchoComponent bot(&eventLoop, &networkFactories);

	eventLoop.run();
	return 0;
}
