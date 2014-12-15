/*
 * Copyright (c) 2012-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/NATPortMapping.h>

namespace Swift {
	class NetworkEnvironment;
	class NATTraverser;
	class NATTraversalGetPublicIPRequest;
	class NATTraversalForwardPortRequest;
	class SOCKS5BytestreamRegistry;
	class ConnectionServerFactory;
	class ConnectionServer;
	class SOCKS5BytestreamServerInitializeRequest;
	class SOCKS5BytestreamServer;

	class SOCKS5BytestreamServerManager {
		public:
			SOCKS5BytestreamServerManager(
					SOCKS5BytestreamRegistry* bytestreamRegistry,
					ConnectionServerFactory* connectionServerFactory, 
					NetworkEnvironment* networkEnvironment,
					NATTraverser* natTraverser);
			~SOCKS5BytestreamServerManager();

			boost::shared_ptr<SOCKS5BytestreamServerInitializeRequest> createInitializeRequest();
			void stop();
			
			std::vector<HostAddressPort> getHostAddressPorts() const;
			std::vector<HostAddressPort> getAssistedHostAddressPorts() const;

			SOCKS5BytestreamServer* getServer() const {
				return server;
			}

		private:
			bool isInitialized() const;
			void initialize();
			void checkInitializeFinished();

			void handleGetPublicIPResult(boost::optional<HostAddress> address);
			void handleForwardPortResult(boost::optional<NATPortMapping> mapping);

			boost::signal<void (bool /* success */)> onInitialized;


		private:
			friend class SOCKS5BytestreamServerInitializeRequest;
			SOCKS5BytestreamRegistry* bytestreamRegistry;
			ConnectionServerFactory* connectionServerFactory;
			NetworkEnvironment* networkEnvironment;
			NATTraverser* natTraverser;
			enum { Start, Initializing, Initialized } state; 
			SOCKS5BytestreamServer* server;
			boost::shared_ptr<ConnectionServer> connectionServer;
			int connectionServerPort;
			boost::shared_ptr<NATTraversalGetPublicIPRequest> getPublicIPRequest;
			boost::shared_ptr<NATTraversalForwardPortRequest> forwardPortRequest;
			boost::optional<HostAddress> publicAddress;
			boost::optional<NATPortMapping> portMapping;
	};
}

