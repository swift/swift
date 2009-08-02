#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "Slimber/ServerError.h"

namespace Swift {
	class DNSSDQuerier;
	class LinkLocalServiceBrowser;
	class VCardCollection;
	class Server;
}

class MenuletController;
class Menulet;

class MainController {
	public:
		MainController(Menulet* menulet);
		virtual ~MainController();

	private:
		void handleSelfConnected(bool b);
		void handleServicesChanged();
		void handleServerStopped(boost::optional<Swift::ServerError> error);

	private:
		Menulet* menulet;
		boost::shared_ptr<Swift::DNSSDQuerier> dnsSDQuerier;
		Swift::LinkLocalServiceBrowser* linkLocalServiceBrowser;
		Swift::VCardCollection* vCardCollection;
		Swift::Server* server;
		MenuletController* menuletController;
};
