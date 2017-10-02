/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>

#include <boost/bind.hpp>

#include <Swiften/Swiften.h>

using namespace Swift;
using namespace boost;

class EchoComponent {
    public:
        EchoComponent(NetworkFactories* networkFactories) : jid("echo.wonderland.lit") {
            component = new Component(jid, "EchoSecret", networkFactories);
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
            message->setFrom(jid);
            component->sendMessage(message);
        }

    private:
        JID jid;
        Component* component;
        ComponentXMLTracer* tracer;
};

int main(int, char**) {
    SimpleEventLoop eventLoop;
    BoostNetworkFactories networkFactories(&eventLoop);

    EchoComponent bot(&networkFactories);

    eventLoop.run();
    return 0;
}
