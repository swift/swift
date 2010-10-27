/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "Slimber/ServerError.h"

namespace Swift {
	class DNSSDQuerier;
	class LinkLocalServiceBrowser;
	class VCardCollection;
	class Server;
	class EventLoop;
}

class MenuletController;
class Menulet;

class MainController {
	public:
		MainController(Menulet* menulet, Swift::EventLoop* eventLoop);
		virtual ~MainController();

	private:
		void handleSelfConnected(bool b);
		void handleServicesChanged();
		void handleServerStopped(boost::optional<Swift::ServerError> error);
		void handleRestartRequested();

		void start();
		void stop();

	private:
		Menulet* menulet;
		boost::shared_ptr<Swift::DNSSDQuerier> dnsSDQuerier;
		Swift::LinkLocalServiceBrowser* linkLocalServiceBrowser;
		Swift::VCardCollection* vCardCollection;
		Swift::Server* server;
		MenuletController* menuletController;
};
