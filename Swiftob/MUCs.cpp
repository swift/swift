/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiftob/MUCs.h>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Client/Client.h>
#include <Swiften/MUC/MUC.h>
#include <Swiften/MUC/MUCManager.h>
#include <Swiften/Base/String.h>

#include <Swiftob/Storage.h>

#define MUC_LIST_SETTING "muc_list"

typedef std::pair<JID, MUC::ref> JIDMUCPair;

MUCs::MUCs(Client* client, Storage* storage) : defaultNick_("Kanchil+") {
	client_ = client;
	storage_ = storage;
	client_->onConnected.connect(boost::bind(&MUCs::handleConnected, this));
}

void MUCs::handleConnected() {
	foreach (std::string room, String::split(storage_->getSetting(MUC_LIST_SETTING), ' ')) {
		join(JID(room), boost::bind(&MUCs::handleInitialJoinSuccess, this), boost::bind(&MUCs::handleInitialJoinFailure, this, _1));
	}
}

void MUCs::handleInitialJoinSuccess() {

}

void MUCs::handleInitialJoinFailure(const std::string&) {

}

void MUCs::join(const JID& room, boost::signal<void (const std::string&)>::slot_type successCallback, boost::function<void(const std::string& /*reason*/)> failureCallback) {
	if (contains(room)) {
		failureCallback("Already in room");
	}
	mucs_[room] = client_->getMUCManager()->createMUC(room);
	mucs_[room]->onJoinComplete.connect(successCallback);
	mucs_[room]->onJoinFailed.connect(boost::bind(&MUCs::handleJoinFailed, this, room, _1, failureCallback));
	mucs_[room]->joinWithContextSince(defaultNick_, boost::posix_time::microsec_clock::universal_time());
	save();
}

void MUCs::part(const JID& room) {
	if (!contains(room)) {
		return;
	}
	mucs_[room]->part();
}

bool MUCs::contains(const JID& room) {
	return mucs_.find(room) != mucs_.end();
}

void MUCs::handleJoinFailed(const JID& muc, ErrorPayload::ref error, boost::function<void(const std::string& /*reason*/)> failureCallback) {
	std::string errorMessage("Couldn't join room");
	if (error) {
		switch (error->getCondition()) {
			case ErrorPayload::Conflict:
			//	rejoinNick = nick_ + "_";
				errorMessage += ": Nickname in use";
			//				errorMessage = str(format("Unable to enter this room as %1%, retrying as %2%") % nick_ % rejoinNick);
				break;
			case ErrorPayload::JIDMalformed:
				errorMessage += ": ";
				errorMessage += "No nickname specified";
				break;
			case ErrorPayload::NotAuthorized:
				errorMessage += ": ";
				errorMessage += "A password needed";
				break;
			case ErrorPayload::RegistrationRequired:
				errorMessage += ": ";
				errorMessage += "Only members may enter";
				break;
			case ErrorPayload::Forbidden:
				errorMessage += ": ";
				errorMessage += "You are banned from the room";
				break;
			case ErrorPayload::ServiceUnavailable:
				errorMessage += ": ";
				errorMessage += "The room is full";
				break;
			case ErrorPayload::ItemNotFound:
				errorMessage += ": ";
				errorMessage += "The room does not exist";
				break;
			default:
				break;
		}
		if (!error->getText().empty()) {
			errorMessage += " - " + error->getText();
		}
	}
	mucs_.erase(muc);
	failureCallback(errorMessage);
}

void MUCs::save() {
	std::string concat;
	foreach (JIDMUCPair pair, mucs_) {
		concat += pair.first.toString() + " ";
	}
	storage_->saveSetting(MUC_LIST_SETTING, concat);
}

MUC::ref MUCs::getMUC(const JID& room) {
	return (mucs_.find(room) != mucs_.end()) ? mucs_[room] : MUC::ref();
}
