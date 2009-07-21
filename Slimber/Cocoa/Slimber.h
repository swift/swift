#pragma once

#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/LinkLocal/LinkLocalRoster.h"

@class Menulet;
namespace Swift {
	class Server;
}

class Slimber {
	public:
		Slimber();
		~Slimber();

	private:
		void handleSelfConnected(bool b);
		void handleRosterChanged();

	private:
		boost::shared_ptr<Swift::DNSSDService> dnsSDService;
		boost::shared_ptr<Swift::LinkLocalRoster>linkLocalRoster;
		Swift::Server* server;
		Menulet* menulet;
};
