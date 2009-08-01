#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "Slimber/ServerError.h"

@class Menulet;
namespace Swift {
	class Server;
	class VCardCollection;
	class LinkLocalServiceBrowser;
	class BonjourQuerier;
}

class Slimber {
	public:
		Slimber();
		~Slimber();

	private:
		void handleSelfConnected(bool b);
		void handleServicesChanged();
		void handleServerStopped(boost::optional<Swift::ServerError> error);

	private:
		boost::shared_ptr<Swift::BonjourQuerier> dnsSDQuerier;
		Swift::LinkLocalServiceBrowser* linkLocalServiceBrowser;
		Swift::VCardCollection* vCardCollection;
		Swift::Server* server;
		Menulet* menulet;
};
