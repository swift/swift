/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_set>

#include <boost/optional.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientOptions.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/MIXParticipant.h>
#include <Swiften/Elements/MIXPayload.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MIX/MIX.h>
#include <Swiften/MIX/MIXImpl.h>
#include <Swiften/MIX/MIXRegistry.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Queries/GenericRequest.h>

using namespace Swift;
using namespace std;

static SimpleEventLoop eventLoop;
static BoostNetworkFactories networkFactories(&eventLoop);

static std::shared_ptr<Client> client;
static MIXImpl::ref mix;
static JID mixChannelJID;
static JID mixServiceDomain;
static std::unordered_set<std::string> supportedNodes;
static std::unordered_set<std::string> participants;
static MIXRegistry* mixRegistry_;

static void handleMessageReceived(Message::ref message) {
    if (auto mixPayload = message->getPayload<MIXPayload>()) {
        if (mixPayload->getSubmissionID()) {
            SWIFT_LOG(debug) << "Ignoring the replicated message" << std::endl;
            return;
        } else if (message->getFrom().toBare() == mixChannelJID) {
            std::cout << "[ " << mixChannelJID << " ] " << message->getFrom().getResource() << ": " << message->getBody().get_value_or("") << std::endl;
        }
    }
}

static void handlePresenceReceived(Presence::ref presence) {
    std::string presenceStatus;
    if (presence->getType() == Presence::Type::Unavailable) {
        presenceStatus = " is now unavailable ";
    } else {
        presenceStatus = " is available ";
    }
    std::cout << "[ " << mixChannelJID << " ] " << ": " << presence->getFrom() << presenceStatus << std::endl;
}

static void handlePresenceChanged(Presence::ref presence) {

    if (presence->getType() == Presence::Type::Unavailable) {
        std::cout << "Client is going offline " << std::endl;
        std::cout << std::endl;
    } else {
        std::cout << "Client is now online " << std::endl;
        std::cout << std::endl;
        // Sending message to channel
        mix->sendMessage("Hello, I am here! " + client->getJID().getNode());

        // Leave channel.
        // Commented to test the sync between different clients of same user.
        //mixRegistry_->leaveChannel(mixChannelJID);
    }
}

static void handleLookupResponse(std::shared_ptr<PubSub> payload, ErrorPayload::ref error) {
    if (error) {
        return;
    }

    std::cout << "Lookup of participant in channel " << mixChannelJID << std::endl;
    int participantCount = 0;
    auto itemsPayload = std::dynamic_pointer_cast<PubSubItems>(payload->getPayload());
    SWIFT_LOG_ASSERT(itemsPayload->getNode() == MIX::JIDMapNode, warning);

    auto items = itemsPayload->getItems();

    for(auto item : items) {
        SWIFT_LOG_ASSERT(item->getData().size() == static_cast<int>(1), warning);
        participantCount++;
        auto mixParticipant = std::dynamic_pointer_cast<MIXParticipant>(item->getData()[0]);
        if (auto realJID = mixParticipant->getJID()) {
            std::cout << "\t" << participantCount << ". " << item->getID() << " - " <<  *realJID << std::endl;
        }
    }

    std::cout << std::endl;

    mix->onPresenceChanged.connect(&handlePresenceChanged);

    // This is just for testing. To test whether client receives channel presence when it comes online after being online for some time.
    if (client->getJID().toBare() == "another@example.com") {
        mix->goOffline();
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        mix->setPresence(Presence::Type::Available);
    } else {
        mix->setPresence(Presence::Type::Available);
    }
}

static void handleParticipantsReceived(std::shared_ptr<PubSub> payload, ErrorPayload::ref error) {
    if (error) {
        return;
    }

    std::cout << "Participants of channel " << mixChannelJID << std::endl;
    int participantCount = 0;
    auto itemsPayload = std::dynamic_pointer_cast<PubSubItems>(payload->getPayload());
    SWIFT_LOG_ASSERT(itemsPayload->getNode() == MIX::ParticipantsNode, warning);

    auto items = itemsPayload->getItems();

    for(auto item : items) {
        SWIFT_LOG_ASSERT(item->getData().size() == static_cast<int>(1), warning);
        participantCount++;
        auto mixParticipant = std::dynamic_pointer_cast<MIXParticipant>(item->getData()[0]);
        SWIFT_LOG_ASSERT(mixParticipant->getNick(), warning);
        std::cout << "\t" << participantCount << ". " << item->getID() << " Nick: [" <<  *mixParticipant->getNick() << "]" << std::endl;
        participants.insert(item->getID());
    }

    if (participantCount == 0) {
        std::cout << "No participants already joined." << std::endl;
    }

    std::cout << std::endl;

    //Perform JID lookups.
    mix->onLookupResponse.connect(&handleLookupResponse);
    mix->lookupJID(participants);
}

static void handleSetNick(MIXSetNick::ref payload, ErrorPayload::ref error) {
    if (error) {
        if (error->getCondition() == ErrorPayload::Condition::Conflict) {
            std::cout << "Nick " << payload->getNick() << " already taken." << std::endl;
        }
        return;
    }

    std::cout << "Nick Assigned: " << payload->getNick() << std::endl;
    std::cout << std::endl;

    mix->onParticipantResponse.connect(&handleParticipantsReceived);
    mix->requestParticipantList();
}

static void handleChannelJoined(const JID& jid) {
    // Successfully joined the channel.
    assert(mixRegistry_);
    assert(mixRegistry_->getMIXInstance(jid));

    std::cout << "Successfully joined channel " << jid << std::endl;

    // trying to rejoin the same channel joined. Should not send the iq request. Will print a warning.
    mixRegistry_->joinChannel(mixChannelJID, supportedNodes);
    std::cout << std::endl;

    mix = mixRegistry_->getMIXInstance(jid);

    mix->onNickResponse.connect(&handleSetNick);
    SWIFT_LOG(debug) << "Requesting Nick: " << client->getJID().getNode() << std::endl;
    mix->setNick(client->getJID().getNode());
}

static void handleChannelJoinFailed(ErrorPayload::ref /*error*/) {
    std::cout << "Channel Join Failed." << std::endl;
}

static void handleChannelLeft(const JID& jid) {
    assert(mixRegistry_);
    assert(!mixRegistry_->getMIXInstance(jid));

    std::cout << "Successfully left channel " << jid << std::endl;
}

static void handleSyncSuccess() {
    auto channels = mixRegistry_->getChannels();
    int channelCount = 0;
    std::cout << "Already Joined channels: " << std::endl;
    for (auto channel : channels) {
        channelCount++;
        std::cout << "\t" << channelCount << ". " << channel->getChannelJID() << std::endl;
    }
    if (channelCount == 0) {
        std::cout << "No channels already joined." << std::endl;
    }
    std::cout << std::endl;
    // Still issuing this join to check for warning.
    mixRegistry_->joinChannel(mixChannelJID, supportedNodes);
    std::cout << std::endl;
}

static void handleChannelNodesSupported(std::shared_ptr<DiscoItems> items, ErrorPayload::ref error) {
    if (error) {
        std::cout << "Error fetching list of nodes for " << mixChannelJID << std::endl;
        return;
    }

    // Successfully received supported nodes.
    int nodeCount = 0;
    std::cout << "Nodes supported by channel " << mixChannelJID << std::endl;
    for (auto&& item : items->getItems()) {
        nodeCount++;
        std::cout << "\t" << nodeCount << ". " << item.getName() << std::endl;
        supportedNodes.insert(item.getName());
    }
    std::cout << std::endl;

    // Initialize MIX Registry and send join request.
    mixRegistry_ = client->getMIXRegistry();
    mixRegistry_->onSyncSuccess.connect(&handleSyncSuccess);
    mixRegistry_->onChannelJoined.connect(&handleChannelJoined);
    mixRegistry_->onChannelJoinFailed.connect(&handleChannelJoinFailed);
    mixRegistry_->onChannelLeft.connect(&handleChannelLeft);
    client->requestRoster();
}

static void handleChannelItemsResponse(std::shared_ptr<DiscoItems> items, ErrorPayload::ref error) {
    if (error) {
        std::cout << "Error fetching list of channels." << std::endl;
        return;
    }

    //Successfully received the list of available channels.
    int channelCount = 0;
    std::cout << "List of rooms at " << mixServiceDomain << std::endl;
    for (auto&& item : items->getItems()) {
        channelCount++;
        std::cout << "\t" << channelCount << ". " << item.getJID().getNode() << " - " << item.getJID() << std::endl;
        if (channelCount == 1) {
            mixChannelJID = item.getJID();
        }
    }
    std::cout << std::endl;

    // Get the list of MIX nodes supported by MIX channel to be joined.
    if (mixChannelJID.isValid()) {
        auto discoItemsRequest = GetDiscoItemsRequest::create(mixChannelJID, std::string("mix"), client->getIQRouter());
        discoItemsRequest->onResponse.connect(&handleChannelNodesSupported);

        std::cout << "Request supported nodes for MIX channel: " << mixChannelJID << std::endl;
        discoItemsRequest->send();
    }
}

static void handleConnected() {
    //Successfully connected.
    std::cout << "Connected" << std::endl;

    auto discoItemsRequest = GetDiscoItemsRequest::create(mixServiceDomain, client->getIQRouter());
    discoItemsRequest->onResponse.connect(&handleChannelItemsResponse);

    //See the list of available channels.
    std::cout << "Request list of channels." << std::endl;
    discoItemsRequest->send();
}

static void handleDisconnected(const boost::optional<ClientError>&) {
    SWIFT_LOG(debug) << "Disconnected" << std::endl;
}

/*
 *  Usage: ./MIXJoin <jid> <password>
 */
int main(int argc, char* argv[]) {
    int ret = 0;
    Log::setLogLevel(Log::Severity::warning);

    if (argc != 4) {
        std::cout << "Usage: ./" << argv[0] << " <jid> <password> <mix_service_domain>" << std::endl;
        ret = -1;
    }
    else {
        mixServiceDomain = JID(argv[3]);
        client = std::make_shared<Client>(JID(argv[1]), string(argv[2]), &networkFactories);
        client->setAlwaysTrustCertificates();

        //ClientXMLTracer* tracer = new ClientXMLTracer(client.get());
        ClientOptions options;
        options.allowPLAINWithoutTLS = true;

        client->onConnected.connect(&handleConnected);
        client->onDisconnected.connect(&handleDisconnected);
        client->onMessageReceived.connect(&handleMessageReceived);
        client->onPresenceReceived.connect(&handlePresenceReceived);

        std::cout << "Connecting..." << std::flush;
        client->connect(options);
        {
            auto timer = networkFactories.getTimerFactory()->createTimer(30000);
            timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));

            auto disconnectTimer = networkFactories.getTimerFactory()->createTimer(25000);
            disconnectTimer->onTick.connect(boost::bind(&Client::disconnect, client.get()));

            timer->start();
            disconnectTimer->start();

            eventLoop.run();
        }
    }
    return ret;
}
