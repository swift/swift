/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/DefaultFileTransferTransporterFactory.h>

#include <Swiften/FileTransfer/DefaultFileTransferTransporter.h>

using namespace Swift;

DefaultFileTransferTransporterFactory::DefaultFileTransferTransporterFactory(
		SOCKS5BytestreamRegistry* s5bRegistry, 
		SOCKS5BytestreamServerManager* s5bServerManager,
		SOCKS5BytestreamProxiesManager* s5bProxiesManager, 
		IDGenerator* idGenerator, 
		ConnectionFactory* connectionFactory, 
		TimerFactory* timerFactory, 
		CryptoProvider* cryptoProvider,
		IQRouter* iqRouter) :
			s5bRegistry(s5bRegistry),
			s5bServerManager(s5bServerManager),
			s5bProxiesManager(s5bProxiesManager),
			idGenerator(idGenerator),
			connectionFactory(connectionFactory),
			timerFactory(timerFactory),
			cryptoProvider(cryptoProvider),
			iqRouter(iqRouter)
{
}

DefaultFileTransferTransporterFactory::~DefaultFileTransferTransporterFactory() {
}

FileTransferTransporter* DefaultFileTransferTransporterFactory::createInitiatorTransporter(
		const JID& initiator, const JID& responder) {
	DefaultFileTransferTransporter* transporter = new DefaultFileTransferTransporter(
		initiator, 
		responder,
		DefaultFileTransferTransporter::Initiator,
		s5bRegistry,
		s5bServerManager,
		s5bProxiesManager,
		idGenerator,
		connectionFactory,
		timerFactory,
		cryptoProvider,
		iqRouter);
	transporter->initialize();
	return transporter;
}

FileTransferTransporter* DefaultFileTransferTransporterFactory::createResponderTransporter(
		const JID& initiator, const JID& responder, const std::string& s5bSessionID) {
	DefaultFileTransferTransporter* transporter = new DefaultFileTransferTransporter(
		initiator, 
		responder,
		DefaultFileTransferTransporter::Initiator,
		s5bRegistry,
		s5bServerManager,
		s5bProxiesManager,
		idGenerator,
		connectionFactory,
		timerFactory,
		cryptoProvider,
		iqRouter);
	transporter->initialize(s5bSessionID);
	return transporter;
}
