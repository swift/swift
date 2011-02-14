/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <vector>


#include "Swiften/Roster/XMPPRosterImpl.h"

using namespace Swift;


enum XMPPRosterEvents {None, Add, Remove, Update};

class XMPPRosterSignalHandler {
public:
	XMPPRosterSignalHandler(XMPPRoster* roster) {
		lastEvent_ = None;
		roster->onJIDAdded.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDAdded, this, _1));
		roster->onJIDRemoved.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDRemoved, this, _1));
		roster->onJIDUpdated.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDUpdated, this, _1, _2, _3));
	}

	XMPPRosterEvents getLastEvent() {
		return lastEvent_;
	}

	JID getLastJID() {
		return lastJID_;
	}

	std::string getLastOldName() {
		return lastOldName_;
	}

	std::vector<std::string> getLastOldGroups() {
		return lastOldGroups_;
	}

	void reset() {
		lastEvent_ = None;
	}

private:
	void handleJIDAdded(const JID& jid) {
		lastJID_ = jid;
		lastEvent_ = Add;
	}

	void handleJIDRemoved(const JID& jid) {
		lastJID_ = jid;
		lastEvent_ = Remove;
	}

	void handleJIDUpdated(const JID& jid, const std::string& oldName, const std::vector<std::string>& oldGroups) {
		CPPUNIT_ASSERT_EQUAL(None, lastEvent_);
		lastJID_ = jid;
		lastOldName_ = oldName;
		lastOldGroups_ = oldGroups;
		lastEvent_ = Update;
	}

	XMPPRosterEvents lastEvent_;
	JID lastJID_;
	std::string lastOldName_;
	std::vector<std::string> lastOldGroups_;

};
