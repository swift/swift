#pragma once

#include <string>
#include <boost/bind.hpp>

#include "Swiften/LinkLocal/DNSSDService.h"

@class Menulet;
namespace Swift {
	class Server;
}

class Slimber {
	public:
		Slimber();
		~Slimber();

	private:
		boost::shared_ptr<Swift::DNSSDService> dnsSDService;
		Swift::Server* server;
		Menulet* menulet;
};
