/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_RosterItem_H
#define SWIFTEN_RosterItem_H

#include "Swiften/Roster/UserRosterAction.h"

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {

class RosterItem {
	public:
		virtual ~RosterItem() {};
		boost::signal<void (boost::shared_ptr<UserRosterAction>)> onUserAction;
	protected:
		void handleUserAction(boost::shared_ptr<UserRosterAction> action) {
			action->setRosterItem(this);
			onUserAction(action);
		}
};

}
#endif

