/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Roster/UserRosterAction.h"

namespace Swift {
class RosterItem;
class TreeWidgetItem;

class RemoveItemRosterAction : public UserRosterAction {
	public:
		virtual ~RemoveItemRosterAction() {};

};

}
