/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>
#include <memory>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Disco/ClientDiscoManager.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/FileTransfer/FileWriteBytestream.h>
#include <Swiften/FileTransfer/IncomingFileTransferManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/Network/BoostNetworkFactories.h>

using namespace Swift;

static SimpleEventLoop eventLoop;
static BoostNetworkFactories networkFactories(&eventLoop);

static int exitCode = 2;

static const std::string CLIENT_NAME = "Swiften FT Test";
static const std::string CLIENT_NODE = "http://swift.im";

class FileReceiver {
    public:
        FileReceiver(const JID& jid, const std::string& password) : jid(jid), password(password) {
            client = new Swift::Client(jid, password, &networkFactories);
            client->onConnected.connect(boost::bind(&FileReceiver::handleConnected, this));
            client->onDisconnected.connect(boost::bind(&FileReceiver::handleDisconnected, this, _1));
            tracer = new ClientXMLTracer(client);
        }

        ~FileReceiver() {
            delete tracer;
            client->onDisconnected.disconnect(boost::bind(&FileReceiver::handleDisconnected, this, _1));
            client->onConnected.disconnect(boost::bind(&FileReceiver::handleConnected, this));
            delete client;
        }

        void start() {
            client->connect();
        }

        void stop() {
            for (const auto& transfer : incomingFileTransfers) {
                (void)transfer;
                //transfer->stop();
            }
            client->disconnect();
        }

    private:
        void handleConnected() {
            Log::setLogLevel(Log::debug);
            client->getFileTransferManager()->onIncomingFileTransfer.connect(boost::bind(&FileReceiver::handleIncomingFileTransfer, this, _1));

            DiscoInfo discoInfo;
            discoInfo.addIdentity(DiscoInfo::Identity(CLIENT_NAME, "client", "pc"));
            discoInfo.addFeature(DiscoInfo::JingleFeature);
            discoInfo.addFeature(DiscoInfo::JingleFTFeature);
            discoInfo.addFeature(DiscoInfo::Bytestream);
            discoInfo.addFeature(DiscoInfo::JingleTransportsIBBFeature);
            discoInfo.addFeature(DiscoInfo::JingleTransportsS5BFeature);
            client->getDiscoManager()->setCapsNode(CLIENT_NODE);
            client->getDiscoManager()->setDiscoInfo(discoInfo);
            client->getPresenceSender()->sendPresence(Presence::create());
        }

        void handleIncomingFileTransfer(IncomingFileTransfer::ref transfer) {
            SWIFT_LOG(debug) << "foo" << std::endl;
            incomingFileTransfers.push_back(transfer);
            std::shared_ptr<FileWriteBytestream> out = std::make_shared<FileWriteBytestream>("out");
            transfer->onFinished.connect(boost::bind(&FileReceiver::handleFileTransferFinished, this, _1, out));
            transfer->accept(out);
        }

        void handleDisconnected(const boost::optional<ClientError>&) {
            std::cerr << "Error!" << std::endl;
            exit(-1);
        }

        void handleFileTransferFinished(
                const boost::optional<FileTransferError>& error,
                std::shared_ptr<FileWriteBytestream> out) {
            std::cout << "File transfer finished" << std::endl;
            out->close();
            if (error) {
                exit(-1);
            }
            else {
                exit(0);
            }
        }

        void exit(int code) {
            exitCode = code;
            stop();
            eventLoop.stop();
        }

    private:
        JID jid;
        std::string password;
        Client* client;
        ClientXMLTracer* tracer;
        std::vector<IncomingFileTransfer::ref> incomingFileTransfers;
};


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <jid> <password>" << std::endl;
        return -1;
    }

    JID jid(argv[1]);
    FileReceiver fileReceiver(jid, std::string(argv[2]));
    fileReceiver.start();

    eventLoop.run();

    return exitCode;
}
