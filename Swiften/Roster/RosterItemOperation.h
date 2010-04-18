/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_RosterItemOperation_H
#define SWIFTEN_RosterItemOperation_H

#include "Swiften/Roster/RosterItem.h"

namespace Swift {

class RosterItemOperation {
	public:
		RosterItemOperation(bool requiresLookup = false, const JID& lookupJID = JID()) : requiresLookup_(requiresLookup), lookupJID_(lookupJID) {};
		virtual ~RosterItemOperation() {};
		bool requiresLookup() const {return requiresLookup_;};
		const JID& lookupJID() const {return lookupJID_;}; 
		virtual void operator() (RosterItem*) const = 0;
	private:
		bool requiresLookup_;
		JID lookupJID_;
};

}
#endif

