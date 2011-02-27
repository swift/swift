/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/MUC/MUC.h>
#include <Swiften/MUC/MUCRegistry.h>

namespace Swift {
class Client;
class MUC;
}

class Storage;

using namespace Swift;

class MUCs {
	public:
		MUCs(Client* client, Storage* storage);
		void join(const JID& room, boost::signal<void (const std::string&)>::slot_type successCallback, boost::function<void(const std::string& /*reason*/)> failureCallback);
		void part(const JID& room);
		bool contains(const JID& room);
		MUC::ref getMUC(const JID& room);
	private:
		void handleConnected();
		void handleJoinFailed(const JID& room, ErrorPayload::ref error, boost::function<void(const std::string& /*reason*/)> failureCallback);
		void handleInitialJoinSuccess();
		void handleInitialJoinFailure(const std::string&);
		void save();
	private:
		MUCRegistry registry_;
		std::map<JID, MUC::ref> mucs_;
		Client* client_;
		Storage* storage_;
		std::string defaultNick_;
};


