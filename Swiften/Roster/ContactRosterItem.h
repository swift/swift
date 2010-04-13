/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ContactRosterItem_H
#define SWIFTEN_ContactRosterItem_H

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/RosterItem.h"
#include "Swiften/Roster/UserRosterAction.h"
#include "Swiften/Elements/StatusShow.h"

#include <boost/bind.hpp>
#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {

class TreeWidgetItem;
class GroupRosterItem;
class ContactRosterItem : public RosterItem {
	public:
		ContactRosterItem(const JID& jid, const String& name, GroupRosterItem* parent, TreeWidgetFactory* factory);
		~ContactRosterItem();

		StatusShow::Type getStatusShow();
		void setStatusShow(StatusShow::Type show);
		void setStatusText(const String& status);
		void setAvatarPath(const String& path);
		const JID& getJID() const;
		void setName(const String& name);
		void show();
		void hide();

	private:
		JID jid_;
		String name_;
		TreeWidgetItem *widget_;
		StatusShow::Type statusShow_;
};

}
#endif

