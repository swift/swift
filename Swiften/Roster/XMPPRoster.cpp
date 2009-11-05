#include "Swiften/Roster/XMPPRoster.h"

namespace Swift {

void XMPPRoster::addContact(const JID& jid, const String& name, const std::vector<String>& groups) {
	JID bareJID(jid.toBare());
	bool exists = containsJID(bareJID);
	if (exists) {
		entries_.erase(bareJID);
	}
	String oldName = getNameForJID(bareJID);
	std::vector<String> oldGroups = entries_[bareJID].second;
	entries_[bareJID] = std::pair<String, std::vector<String> >(name, groups);
	if (exists) {
		
		onJIDUpdated(bareJID, oldName, oldGroups);
	} else {
		onJIDAdded(bareJID);
	}
}

void XMPPRoster::removeContact(const JID& jid) {
	entries_.erase(JID(jid.toBare()));
	onJIDRemoved(jid);
}

bool XMPPRoster::containsJID(const JID& jid) {
	return entries_.find(JID(jid.toBare())) != entries_.end();
}

const String& XMPPRoster::getNameForJID(const JID& jid) {
	return entries_[JID(jid.toBare())].first;
}

const std::vector<String>& XMPPRoster::getGroupsForJID(const JID& jid) {
	return entries_[JID(jid.toBare())].second;
}

}

