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

class MUCs {
	public:
		MUCs(Swift::Client* client, Storage* storage);
		void join(const Swift::JID& room, boost::signal<void (const std::string&)>::slot_type successCallback, boost::function<void(const std::string& /*reason*/)> failureCallback);
		void part(const Swift::JID& room);
		bool contains(const Swift::JID& room);
		Swift::MUC::ref getMUC(const Swift::JID& room);
		const std::string& getDefaultNick() const {return defaultNick_;}
		bool setDefaultNick(const std::string& nick);
	private:
		void handleConnected();
		void handleJoinFailed(const Swift::JID& room, Swift::ErrorPayload::ref error, boost::function<void(const std::string& /*reason*/)> failureCallback);
		void handleInitialJoinSuccess();
		void handleInitialJoinFailure(const std::string&);
		void save();
	private:
		Swift::MUCRegistry registry_;
		std::map<Swift::JID, Swift::MUC::ref> mucs_;
		Swift::Client* client_;
		Storage* storage_;
		std::string defaultNick_;
};


