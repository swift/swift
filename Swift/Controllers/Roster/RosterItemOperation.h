/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Roster/RosterItem.h"

namespace Swift {

class RosterItemOperation {
	public:
		RosterItemOperation(bool requiresLookup = false, const JID& lookupJID = JID()) : requiresLookup_(requiresLookup), lookupJID_(lookupJID) {};
		virtual ~RosterItemOperation() {};
		bool requiresLookup() const {return requiresLookup_;};
		const JID& lookupJID() const {return lookupJID_;}; 
		/**
		 * This is called when iterating over possible subjects, so must check it's
		 * applying to the right items - even if requiresLookup() is true an item
		 * with the same bare JID but different full JID may be passed.
		 */
		virtual void operator() (RosterItem*) const = 0;
	private:
		bool requiresLookup_;
		JID lookupJID_;
};

}
