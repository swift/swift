/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <Swiften/Roster/XMPPRosterImpl.h>

enum XMPPRosterEvents {None, Add, Remove, Update};

class XMPPRosterSignalHandler {
public:
	XMPPRosterSignalHandler(Swift::XMPPRoster* roster);

	XMPPRosterEvents getLastEvent() {
		return lastEvent_;
	}

	Swift::JID getLastJID() {
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

	int getEventCount() const {
		return eventCount;
	}

private:
	void handleJIDAdded(const Swift::JID& jid) {
		lastJID_ = jid;
		lastEvent_ = Add;
		eventCount++;
	}

	void handleJIDRemoved(const Swift::JID& jid) {
		lastJID_ = jid;
		lastEvent_ = Remove;
		eventCount++;
	}

	void handleJIDUpdated(const Swift::JID& jid, const std::string& oldName, const std::vector<std::string>& oldGroups);

	XMPPRosterEvents lastEvent_;
	Swift::JID lastJID_;
	std::string lastOldName_;
	std::vector<std::string> lastOldGroups_;
	int eventCount;
};
