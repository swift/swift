/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_GroupRosterItem_H
#define SWIFTEN_GroupRosterItem_H

#include "Swiften/Roster/RosterItem.h"
#include "Swiften/Base/String.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "Swiften/Roster/ContactRosterItem.h"

#include <list>

namespace Swift {

class GroupRosterItem : public RosterItem {
	public:
		GroupRosterItem(const String& name, TreeWidget* tree, TreeWidgetFactory* factory) : name_(name) {
			widget_ = factory->createTreeWidgetItem(tree);
			widget_->setExpanded(true);
			widget_->setText(name);
		}

		~GroupRosterItem() {
			delete widget_;
		}

		const String& getName() const {
			return name_;
		}

		TreeWidgetItem* getWidget() const {
			return widget_;
		}

		const std::list<RosterItem*>& getChildren() const {
			return children_;
		}

		void addChild(RosterItem* item) {
			children_.push_back(item);
		}

		void removeChild(const JID& jid) {
			std::list<RosterItem*>::iterator it = children_.begin();
			while (it != children_.end()) {
				ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(*it);
				if (contact && contact->getJID() == jid) {
					delete contact;
					it = children_.erase(it);
					continue;
				} 
				GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
				if (group) {
					group->removeChild(jid);
				}
				it++;
			}
		}

	private:
		String name_;
		TreeWidgetItem* widget_;
		std::list<RosterItem*> children_;
};

}
#endif

