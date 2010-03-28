#pragma once

#include <vector>
#include <boost/signal.hpp>

#include "Swiften/Base/String.h"

class Menulet;

class MenuletController {
	public:
		enum Status {
			Online,
			Offline
		};

		MenuletController(Menulet*);
		virtual ~MenuletController();

		void setXMPPStatus(const Swift::String& message, Status status);
		void setUserNames(const std::vector<Swift::String>&);
	
		boost::signal<void ()> onRestartRequested;

	private:
		void update();
	
	private:
		Menulet* menulet;
		Status xmppStatus;
		Swift::String xmppStatusMessage;
		std::vector<Swift::String> linkLocalUsers;
};
