/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_UserRosterAction_H
#define SWIFTEN_UserRosterAction_H

namespace Swift {
class RosterItem;
class TreeWidgetItem;

class UserRosterAction {
	public:
		virtual ~UserRosterAction() {};
		void setRosterItem(RosterItem *item) {
			rosterItem_ = item;
		};
		void setTreeWidgetItem(TreeWidgetItem *item) {
			treeWidgetItem_ = item;
		}
		RosterItem* getRosterItem() {
			return rosterItem_;
		}
		TreeWidgetItem* getTreeWidgetItem() {
			return treeWidgetItem_;
		}

	private:
		RosterItem *rosterItem_;
		TreeWidgetItem *treeWidgetItem_;
};

}
#endif


