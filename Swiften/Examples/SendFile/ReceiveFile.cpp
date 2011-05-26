/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <iostream>

#include <Swiften/Elements/Presence.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Disco/ClientDiscoManager.h>
#include <Swiften/FileTransfer/IncomingFileTransferManager.h>
#include <Swiften/FileTransfer/FileWriteBytestream.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/FileTransfer/DefaultLocalJingleTransportCandidateGeneratorFactory.h>
#include <Swiften/FileTransfer/DefaultRemoteJingleTransportCandidateSelectorFactory.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/FileTransferManager.h>

using namespace Swift;

SimpleEventLoop eventLoop;
BoostNetworkFactories networkFactories(&eventLoop);

int exitCode = 2;

static const std::string CLIENT_NAME = "Swiften FT Test";
static const std::string CLIENT_NODE = "http://swift.im";

class FileReceiver {
	public:
		FileReceiver(const JID& jid, const std::string& password) : jid(jid), password(password), jingleSessionManager(NULL), incomingFileTransferManager(NULL) {
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
			foreach(const IncomingFileTransfer::ref transfer, incomingFileTransfers) {
				//transfer->stop();
			}
			client->disconnect();
		}

	private:
		void handleConnected() {
			Swift::logging = true;
			client->getFileTransferManager()->startListeningOnPort(9999);
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
			transfer->accept(boost::make_shared<FileWriteBytestream>("out"));
			//transfer->onFinished.connect(boost::bind(&FileReceiver::handleFileTransferFinished, this, _1));
			//transfer->start();
		}

		void handleDisconnected(const boost::optional<ClientError>&) {
			std::cerr << "Error!" << std::endl;
			exit(-1);
		}

		/*
		void handleFileTransferFinished(const boost::optional<FileTransferError>& error) {
			std::cout << "File transfer finished" << std::endl;
			if (error) {
				exit(-1);
			}
			else {
				exit(0);
			}
		}*/

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
		JingleSessionManager* jingleSessionManager;
		IncomingFileTransferManager* incomingFileTransferManager;
		std::vector<IncomingFileTransfer::ref> incomingFileTransfers;
		DefaultLocalJingleTransportCandidateGeneratorFactory *localFactory;
		DefaultRemoteJingleTransportCandidateSelectorFactory *remoteFactory;
		SOCKS5BytestreamRegistry* bytestreamRegistry;
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
