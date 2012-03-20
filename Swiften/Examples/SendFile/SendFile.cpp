/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

#include <Swiften/Client/Client.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Network/BoostTimer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/FileTransfer/OutgoingSIFileTransfer.h>
#include <Swiften/FileTransfer/FileReadBytestream.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServer.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/FileTransfer/OutgoingFileTransferManager.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/FileTransfer/DefaultLocalJingleTransportCandidateGeneratorFactory.h>
#include <Swiften/FileTransfer/DefaultRemoteJingleTransportCandidateSelectorFactory.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/StringCodecs/MD5.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/FileTransfer/FileTransferManager.h>

using namespace Swift;

SimpleEventLoop eventLoop;
BoostNetworkFactories networkFactories(&eventLoop);

int exitCode = 2;

class FileSender {
	public:
	FileSender(const JID& jid, const std::string& password, const JID& recipient, const boost::filesystem::path& file) : jid(jid), password(password), recipient(recipient), file(file) {

			client = new Swift::Client(jid, password, &networkFactories);
			client->onConnected.connect(boost::bind(&FileSender::handleConnected, this));
			client->onDisconnected.connect(boost::bind(&FileSender::handleDisconnected, this, _1));
			tracer = new ClientXMLTracer(client);
			client->getEntityCapsProvider()->onCapsChanged.connect(boost::bind(&FileSender::handleCapsChanged, this, _1));
		}

		~FileSender() {
			delete tracer;
			client->onDisconnected.disconnect(boost::bind(&FileSender::handleDisconnected, this, _1));
			client->onConnected.disconnect(boost::bind(&FileSender::handleConnected, this));
			delete client;
		}
		
		void start() {
			client->connect();
		}

	private:
		void handleConnected() {
			client->sendPresence(Presence::create());

			client->getFileTransferManager()->startListeningOnPort(19999);
			//ByteArray fileData;
			//readByteArrayFromFile(fileData, file.string());
			
			// gather file information
			/*StreamInitiationFileInfo fileInfo;
			
			fileInfo.setName(file.filename());
			fileInfo.setSize(boost::filesystem::file_size(file));
			fileInfo.setDescription("Some file!");
			fileInfo.setDate(boost::posix_time::from_time_t(boost::filesystem::last_write_time(file)));*/
			//fileInfo.setHash(Hexify::hexify(MD5::getHash(fileData)));
			/*
			transfer = new OutgoingSIFileTransfer("myid",	client->getJID(), recipient, file.filename(), boost::filesystem::file_size(file), "A file", boost::make_shared<FileReadBytestream>(file)), client->getIQRouter(), socksBytestreamServer);
			transfer->onFinished.connect(boost::bind(&FileSender::handleFileTransferFinished, this, _1));
			transfer->start();
			 */
		}

		void handleCapsChanged(JID jid) {
			if (jid.toBare() == recipient) {
				// create ReadBytestream from file
				boost::shared_ptr<FileReadBytestream> fileStream = boost::make_shared<FileReadBytestream>(file);

				outgoingFileTransfer = client->getFileTransferManager()->createOutgoingFileTransfer(recipient, file, "Some File!", fileStream);

				if (outgoingFileTransfer) {
					std::cout << "started FT" << std::endl;
					outgoingFileTransfer->start();
					// TODO: getting notified about FT status and end
				} else {
					std::cout << "[ ERROR ] " << recipient << " doesn't support any kind of file transfer!" << std::endl;
					//client->disconnect();
				}
			}
		}

		void handleDisconnected(const boost::optional<ClientError>&) {
			std::cerr << "Error!" << std::endl;
			exit(-1);
		}

		void handleFileTransferFinished(const boost::optional<FileTransferError>& error) {
			std::cout << "File transfer finished" << std::endl;
			if (error) {
				exit(-1);
			}
			else {
				exit(0);
			}
		}
		
		void exit(int code) {
			exitCode = code;
			eventLoop.stop();
		}
	
	private:
		BoostConnectionServer::ref connectionServer;
		SOCKS5BytestreamServer* socksBytestreamServer;
		SOCKS5BytestreamRegistry* registry;
		OutgoingFileTransfer::ref outgoingFileTransfer;
		JID jid;
		std::string password;
		JID recipient;
		boost::filesystem::path file;
		Client* client;
		ClientXMLTracer* tracer;
};


int main(int argc, char* argv[]) {
	if (argc != 5) {
		std::cerr << "Usage: " << argv[0] << " <jid> <password> <recipient> <file>" << std::endl;
		return -1;
	}

	JID sender(argv[1]);
	JID recipient(argv[3]);
	Swift::logging = true;
	FileSender fileSender(sender, std::string(argv[2]), recipient, boost::filesystem::path(argv[4]));
	fileSender.start();
	{
		/*BoostTimer::ref timer(BoostTimer::create(30000, &MainBoostIOServiceThread::getInstance().getIOService()));
		timer->onTick.connect(boost::bind(&SimpleEventLoop::stop, &eventLoop));
		timer->start();*/

		eventLoop.run();
	}

	return exitCode;
}
