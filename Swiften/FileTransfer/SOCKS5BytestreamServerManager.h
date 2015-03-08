/*
 * Copyright (c) 2012-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/NATPortMapping.h>

namespace Swift {
	class NetworkEnvironment;
	class NATTraverser;
	class NATTraversalGetPublicIPRequest;
	class NATTraversalForwardPortRequest;
	class NATTraversalRemovePortForwardingRequest;
	class SOCKS5BytestreamRegistry;
	class ConnectionServerFactory;
	class ConnectionServer;
	class SOCKS5BytestreamServer;
	class SOCKS5BytestreamServerResourceUser;
	class SOCKS5BytestreamServerPortForwardingUser;

	class SOCKS5BytestreamServerManager {
		friend class SOCKS5BytestreamServerResourceUser;
		friend class SOCKS5BytestreamServerPortForwardingUser;

		public:
			SOCKS5BytestreamServerManager(
					SOCKS5BytestreamRegistry* bytestreamRegistry,
					ConnectionServerFactory* connectionServerFactory, 
					NetworkEnvironment* networkEnvironment,
					NATTraverser* natTraverser);
			~SOCKS5BytestreamServerManager();

			boost::shared_ptr<SOCKS5BytestreamServerResourceUser> aquireResourceUser();
			boost::shared_ptr<SOCKS5BytestreamServerPortForwardingUser> aquirePortForwardingUser();

			void stop();
			
			std::vector<HostAddressPort> getHostAddressPorts() const;
			std::vector<HostAddressPort> getAssistedHostAddressPorts() const;

			SOCKS5BytestreamServer* getServer() const {
				return server;
			}

		private:
			bool isInitialized() const;
			void initialize();

			bool isPortForwardingReady() const;
			void setupPortForwarding();
			void removePortForwarding();

			void checkInitializeFinished();

			void handleGetPublicIPResult(boost::optional<HostAddress> address);
			void handleForwardPortResult(boost::optional<NATPortMapping> mapping);
			void handleUnforwardPortResult(boost::optional<bool> result);

			boost::signal<void (bool /* success */)> onInitialized;
			boost::signal<void (bool /* success */)> onPortForwardingSetup;

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
			boost::shared_ptr<NATTraversalRemovePortForwardingRequest> unforwardPortRequest;
			boost::optional<HostAddress> publicAddress;
			boost::optional<NATPortMapping> portMapping;
			bool attemptedPortMapping_;

			boost::weak_ptr<SOCKS5BytestreamServerResourceUser> s5bServerResourceUser_;
			boost::weak_ptr<SOCKS5BytestreamServerPortForwardingUser> s5bServerPortForwardingUser_;
	};
}

