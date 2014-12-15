/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>
#include <string>

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Message.h>

namespace Swift {
class Client;
}

class MUCs;

class Users {
	public:
		class User {
			public:
				/* If you add a role here, edit the role lists in Commands.cpp*/
				enum Role {Unknown, Owner};
				User(const Swift::JID& jid, Role role) : jid_(jid), role_(role) {}
				Role getRole() {return role_;}
				Swift::JID getJID() {return jid_;}
			private:
				Swift::JID jid_;
				Role role_;
		};

	public:
		Users(Swift::Client* client, MUCs* mucs);
		void clearAll();
		void addUser(const User& user);
		User::Role getRoleForSender(Swift::Message::ref message);

	private:
		std::vector<User> users_;
		Swift::Client* client_;
		MUCs* mucs_;
};


