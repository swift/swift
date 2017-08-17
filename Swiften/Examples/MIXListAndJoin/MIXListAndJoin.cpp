/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <iostream>
#include <memory>
#include <unordered_set>

#include <boost/optional.hpp>

#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/MIXParticipant.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientOptions.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Disco/GetDiscoItemsRequest.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MIX/MIX.h>
#include <Swiften/MIX/MIXImpl.h>
#include <Swiften/MIX/MIXRegistry.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Queries/GenericRequest.h>

#include <Swiften/Base/Log.h>

using namespace Swift;
using namespace std;

static SimpleEventLoop eventLoop;
static BoostNetworkFactories networkFactories(&eventLoop);

static std::shared_ptr<Client> client;
static MIXImpl::ref mix;
static JID mixChannelJID;
static JID mixServiceDomain;
static std::unordered_set<std::string> supportedNodes;
static MIXRegistry* mixRegistry_;

static void handleChannelJoined(const JID& jid) {
    // Successfully joined the channel.
    assert(mixRegistry_);
    assert(mixRegistry_->getMIXInstance(jid));

    std::cout << "Successfully joined channel " << jid << std::endl;

    //trying to rejoin the same channel joined. Should not send the iq request. Will print a warning.
    mixRegistry_->joinChannel(mixChannelJID, supportedNodes);

    mix = mixRegistry_->getMIXInstance(jid);

    // Can perform other functions with MIXImpl object: mix.

    // Finally leave channel.
    // This is commented to check the sync between different users using different clients(resources) to connect.
    //mixRegistry_->leaveChannel(mixChannelJID);
}

static void handleChannelJoinFailed(ErrorPayload::ref /*error*/) {
    std::cout << "Channel Join Failed." << std::endl;
}

static void handleChannelLeft(const JID& jid) {
    assert(mixRegistry_);
    assert(!mixRegistry_->getMIXInstance(jid));

    std::cout << "Successfully left channel " << jid << std::endl;
}

static void handleRosterPopulated() {
    XMPPRoster* xmppRoster = client->getRoster();
    int channelCount = 0;
    std::cout << "Already Joined channels: " << std::endl;
    for (auto item : xmppRoster->getItems()) {
        channelCount++;
        std::cout << "\t" << channelCount << ". " << item.getName() << " - " << item.getJID() << std::endl;
    }
    if (channelCount == 0) {
        std::cout << "No channels already joined." << std::endl;
    }
    std::cout << std::endl;
    // Still issuing this join to check for warning.
    mixRegistry_->joinChannel(mixChannelJID, supportedNodes);
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
    mixRegistry_->onChannelJoined.connect(&handleChannelJoined);
    mixRegistry_->onChannelJoinFailed.connect(&handleChannelJoinFailed);
    mixRegistry_->onChannelLeft.connect(&handleChannelLeft);

    XMPPRoster* xmppRoster = client->getRoster();
    xmppRoster->onInitialRosterPopulated.connect(&handleRosterPopulated);
    client->requestRoster(true);
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

        std::cout << "Connecting..." << std::flush;
        client->connect(options);
        {
            Timer::ref timer = networkFactories.getTimerFactory()->createTimer(30000);
            timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));

            Timer::ref disconnectTimer = networkFactories.getTimerFactory()->createTimer(25000);
            disconnectTimer->onTick.connect(boost::bind(&Client::disconnect, client.get()));

            timer->start();
            disconnectTimer->start();

            eventLoop.run();
        }
    }
    return ret;
}
