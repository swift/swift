#ifndef SWIFTEN_XMPPRoster_H
#define SWIFTEN_XMPPRoster_H

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

#include <map>
#include <vector>
#include <boost/signal.hpp>

namespace Swift {

class XMPPRoster {
	public:
		XMPPRoster() {};
		~XMPPRoster() {};

		void addContact(const JID& jid, const String& name, const std::vector<String>& groups);
		bool containsJID(const JID& jid); 
		void removeContact(const JID& jid);
		const String& getNameForJID(const JID& jid);
		const std::vector<String>& getGroupsForJID(const JID& jid);

		boost::signal<void (const JID&)> onJIDAdded;
		boost::signal<void (const JID&)> onJIDRemoved;
		boost::signal<void (const JID&, const String&, const std::vector<String>&)> onJIDUpdated;

	private:
		std::map<JID, std::pair<String, std::vector<String> > > entries_;
};
}

#endif

