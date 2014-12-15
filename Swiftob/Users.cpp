/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiftob/Users.h>

#include <iostream>

#include <Swiften/Base/foreach.h>
#include <Swiften/Client/Client.h>

#include <Swiftob/MUCs.h>

using namespace Swift;

Users::Users(Client* client, MUCs* mucs) {
	client_ = client;
	mucs_ = mucs;
}

/* TODO: Store in roster */
void Users::clearAll() {
	users_.clear();
}

void Users::addUser(const User& user) {
	users_.push_back(user);
}

Users::User::Role Users::getRoleForSender(Message::ref message) {
	JID jid = message->getFrom();
	MUC::ref muc = mucs_->getMUC(message->getFrom().toBare());
	if (muc && muc->hasOccupant(message->getFrom().getResource())) {
		MUCOccupant occupant = muc->getOccupant(message->getFrom().getResource());
		if (occupant.getRealJID()) {
			jid = occupant.getRealJID().get();
		}
	}
	foreach (User user, users_) {
		if (user.getJID().equals(jid.toBare(), JID::WithoutResource)) {
			return user.getRole();
		}
	}
	return User::Unknown;
}

