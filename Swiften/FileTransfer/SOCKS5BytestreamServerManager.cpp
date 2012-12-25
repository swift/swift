/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>

#include <Swiften/FileTransfer/SOCKS5BytestreamServerInitializeRequest.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Log.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServer.h>
#include <Swiften/Network/ConnectionServer.h>
#include <Swiften/Network/ConnectionServerFactory.h>
#include <Swiften/Network/NetworkEnvironment.h>
#include <Swiften/Network/NATTraverser.h>
#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/NATTraversalForwardPortRequest.h>

using namespace Swift;

static const int LISTEN_PORTS_BEGIN = 10000;
static const int LISTEN_PORTS_END = 11000;

SOCKS5BytestreamServerManager::SOCKS5BytestreamServerManager(
		SOCKS5BytestreamRegistry* bytestreamRegistry,
		ConnectionServerFactory* connectionServerFactory, 
		NetworkEnvironment* networkEnvironment,
		NATTraverser* natTraverser) :
			bytestreamRegistry(bytestreamRegistry),
			connectionServerFactory(connectionServerFactory),
			networkEnvironment(networkEnvironment),
			natTraverser(natTraverser),
			state(Start),
			server(NULL) {
}

SOCKS5BytestreamServerManager::~SOCKS5BytestreamServerManager() {
	SWIFT_LOG_ASSERT(!connectionServer, warning) << std::endl;
	SWIFT_LOG_ASSERT(!getPublicIPRequest, warning) << std::endl;
	SWIFT_LOG_ASSERT(!forwardPortRequest, warning) << std::endl;
	SWIFT_LOG_ASSERT(state == Start, warning) << std::endl;
}


boost::shared_ptr<SOCKS5BytestreamServerInitializeRequest> SOCKS5BytestreamServerManager::createInitializeRequest() {
	return boost::make_shared<SOCKS5BytestreamServerInitializeRequest>(this);
}

void SOCKS5BytestreamServerManager::stop() {
	if (getPublicIPRequest) {
		getPublicIPRequest->stop();
		getPublicIPRequest.reset();
	}
	if (forwardPortRequest) {
		forwardPortRequest->stop();
		forwardPortRequest.reset();
	}
	if (connectionServer) {
		connectionServer->stop();
		connectionServer.reset();
	}
	// TODO: Remove port forwards
	
	state = Start;
}

std::vector<HostAddressPort> SOCKS5BytestreamServerManager::getHostAddressPorts() const {
	std::vector<HostAddressPort> result;
	if (connectionServer) {
		std::vector<NetworkInterface> networkInterfaces = networkEnvironment->getNetworkInterfaces();
		foreach (const NetworkInterface& networkInterface, networkInterfaces) {
			foreach (const HostAddress& address, networkInterface.getAddresses()) {
				result.push_back(HostAddressPort(address, connectionServerPort));
			}
		}
	}
	return result;
}

std::vector<HostAddressPort> SOCKS5BytestreamServerManager::getAssistedHostAddressPorts() const {
	std::vector<HostAddressPort> result;
	if (publicAddress && portMapping) {
		result.push_back(HostAddressPort(*publicAddress, portMapping->getPublicPort()));
	}
	return result;
}

bool SOCKS5BytestreamServerManager::isInitialized() const {
	return state == Initialized;
}

void SOCKS5BytestreamServerManager::initialize() {
	if (state == Start) {
		state = Initializing;

		// Find a port to listen on
		assert(!connectionServer);
		int port;
		for (port = LISTEN_PORTS_BEGIN; port < LISTEN_PORTS_END; ++port) {
			SWIFT_LOG(debug) << "Trying to start server on port " << port << std::endl;
			connectionServer = connectionServerFactory->createConnectionServer(HostAddress("0.0.0.0"), port);
			boost::optional<ConnectionServer::Error> error = connectionServer->tryStart();
			if (!error) {
				break;
			}
			else if (*error != ConnectionServer::Conflict) {
				SWIFT_LOG(debug) << "Error starting server" << std::endl;
				onInitialized(false);
				return;
			}
			connectionServer.reset();
		}
		if (!connectionServer) {
			SWIFT_LOG(debug) << "Unable to find an open port" << std::endl;
			onInitialized(false);
			return;
		}
		SWIFT_LOG(debug) << "Server started succesfully" << std::endl;
		connectionServerPort = port;

		// Start bytestream server. Should actually happen before the connectionserver is started
		// but that doesn't really matter here.
		assert(!server);
		server = new SOCKS5BytestreamServer(connectionServer, bytestreamRegistry);
		server->start();

		// Retrieve public addresses
		assert(!getPublicIPRequest);
		publicAddress = boost::optional<HostAddress>();
		if ((getPublicIPRequest = natTraverser->createGetPublicIPRequest())) {
			getPublicIPRequest->onResult.connect(
					boost::bind(&SOCKS5BytestreamServerManager::handleGetPublicIPResult, this, _1));
			getPublicIPRequest->start();
		}

		// Forward ports
		assert(!forwardPortRequest);
		portMapping = boost::optional<NATPortMapping>();
		if ((forwardPortRequest = natTraverser->createForwardPortRequest(port, port))) {
			forwardPortRequest->onResult.connect(
					boost::bind(&SOCKS5BytestreamServerManager::handleForwardPortResult, this, _1));
			forwardPortRequest->start();
		}
	}
}

void SOCKS5BytestreamServerManager::handleGetPublicIPResult(boost::optional<HostAddress> address) {
	if (address) {
		SWIFT_LOG(debug) << "Public IP discovered as " << address.get().toString() << "." << std::endl;
	} 
	else {
		SWIFT_LOG(debug) << "No public IP discoverable." << std::endl;
	}

	publicAddress = address;

	getPublicIPRequest->stop();
	getPublicIPRequest.reset();

	checkInitializeFinished();
}

void SOCKS5BytestreamServerManager::handleForwardPortResult(boost::optional<NATPortMapping> mapping) {
	if (mapping) {
		SWIFT_LOG(debug) << "Mapping port was successful." << std::endl;
	} 
	else {
		SWIFT_LOG(debug) << "Mapping port has failed." << std::endl;
	}

	portMapping = mapping;

	forwardPortRequest->stop();
	forwardPortRequest.reset();

	checkInitializeFinished();
}

void SOCKS5BytestreamServerManager::checkInitializeFinished() {
	assert(state == Initializing);
	if (!getPublicIPRequest && !forwardPortRequest) {
		state = Initialized;
		onInitialized(true);
	}
}
