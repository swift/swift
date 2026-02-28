/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/FileTransfer/FileReadBytestream.h>
#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/FileTransfer/OutgoingFileTransferManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServer.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/BoostTimer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/StringCodecs/Hexify.h>

using namespace Swift;

static SimpleEventLoop eventLoop;
static BoostNetworkFactories networkFactories(&eventLoop);

static int exitCode = 2;

class FileSender {
    public:
    FileSender(const JID& jid, const std::string& password, const JID& recipient, const boost::filesystem::path& file) : jid(jid), password(password), recipient(recipient), file(file), tracer(nullptr), client(std::unique_ptr<Swift::Client>(new Swift::Client(jid, password, &networkFactories))) {
            client->onConnected.connect(boost::bind(&FileSender::handleConnected, this));
            client->onDisconnected.connect(boost::bind(&FileSender::handleDisconnected, this, _1));
            //tracer = new ClientXMLTracer(client);
            client->getEntityCapsProvider()->onCapsChanged.connect(boost::bind(&FileSender::handleCapsChanged, this, _1));
        }

        ~FileSender() {
            delete tracer;
            client->onDisconnected.disconnect(boost::bind(&FileSender::handleDisconnected, this, _1));
            client->onConnected.disconnect(boost::bind(&FileSender::handleConnected, this));
        }

        void start() {
            client->connect();
        }

    private:
        void handleConnected() {
            std::cout << "Connected. Awaiting presence from receipient." << std::endl;
            client->sendPresence(Presence::create());
        }

        void handleCapsChanged(JID jid) {
            if (jid.toBare() == recipient) {
                // create ReadBytestream from file
                std::shared_ptr<FileReadBytestream> fileStream = std::make_shared<FileReadBytestream>(file);

                outgoingFileTransfer = client->getFileTransferManager()->createOutgoingFileTransfer(recipient, file, "Some File!", fileStream);

                if (outgoingFileTransfer) {
                    outgoingFileTransfer->onFinished.connect(boost::bind(&FileSender::handleFileTransferFinished, this, _1));
                    std::cout << "Starting file-transfer to " << recipient.toString() << "." << std::endl;
                    outgoingFileTransfer->start();
                    // TODO: getting notified about FT status and end
                } else {
                    std::cout << recipient << " doesn't support any kind of file transfer!" << std::endl;
                    client->disconnect();
                }
            }
        }

        void handleDisconnected(const boost::optional<ClientError>& err) {
            if (err) {
                std::cout << "Disconnected due to error ( " << err.get().getType() << " )." << std::endl;
                exit(-1);
            }
            else {
                std::cout << "Successfully disconnected." << std::endl;
            }
        }

        void handleFileTransferFinished(const boost::optional<FileTransferError>& error) {
            std::cout << "File transfer finished." << std::endl;
            if (error) {
                client->disconnect();
                exit(-1);
            }
            else {
                client->disconnect();
                exit(0);
            }
        }

        void exit(int code) {
            exitCode = code;
            eventLoop.stop();
        }

    private:
        BoostConnectionServer::ref connectionServer;
        OutgoingFileTransfer::ref outgoingFileTransfer;
        JID jid;
        std::string password;
        JID recipient;
        boost::filesystem::path file;
        ClientXMLTracer* tracer;
        std::unique_ptr<Swift::Client> client;
};


int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <jid> <password> <recipient> <file>" << std::endl;
        return -1;
    }

    //Log::setLogLevel(Log::debug);

    JID sender(argv[1]);
    JID recipient(argv[3]);
    FileSender fileSender(sender, std::string(argv[2]), recipient, boost::filesystem::path(argv[4]));
    fileSender.start();
    {
        Timer::ref timer = networkFactories.getTimerFactory()->createTimer(30000);
        timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
        timer->start();

        eventLoop.run();
    }

    return exitCode;
}
