/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Roster/UnitTest/XMPPRosterSignalHandler.h>

#include <boost/bind.hpp>
#include <cassert>

using namespace Swift;

XMPPRosterSignalHandler::XMPPRosterSignalHandler(Swift::XMPPRoster* roster) : eventCount(0) {
	lastEvent_ = None;
	roster->onJIDAdded.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDAdded, this, _1));
	roster->onJIDRemoved.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDRemoved, this, _1));
	roster->onJIDUpdated.connect(boost::bind(&XMPPRosterSignalHandler::handleJIDUpdated, this, _1, _2, _3));
}

void XMPPRosterSignalHandler::handleJIDUpdated(const Swift::JID& jid, const std::string& oldName, const std::vector<std::string>& oldGroups) {
	assert(lastEvent_ == None);
	lastJID_ = jid;
	lastOldName_ = oldName;
	lastOldGroups_ = oldGroups;
	lastEvent_ = Update;
	eventCount++;
}
