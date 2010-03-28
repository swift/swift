#ifndef SWIFTEN_Roster_H
#define SWIFTEN_Roster_H

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Roster/RosterItemOperation.h"
#include "Swiften/Roster/UserRosterAction.h"
#include "Swiften/Roster/RosterFilter.h"

#include <vector>
#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {

class TreeWidgetFactory;
class TreeWidget;
class RosterItem;
class GroupRosterItem;

class Roster {
	public:
		Roster(TreeWidget *treeWidget, TreeWidgetFactory *widgetFactory);
		~Roster();

		TreeWidget* getWidget();
		GroupRosterItem* getGroup(const String& groupName);
		void addContact(const JID& jid, const String& name, const String& group);
		void removeContact(const JID& jid);
		void removeContactFromGroup(const JID& jid, const String& group);
		void applyOnItems(const RosterItemOperation& operation);
		boost::signal<void (boost::shared_ptr<UserRosterAction>)> onUserAction;
		void addFilter(RosterFilter *filter) {filters_.push_back(filter);filterAll();}
		void removeFilter(RosterFilter *filter);
		std::vector<RosterFilter*> getFilters() {return filters_;}

	private:
		void filterItem(RosterItem* item);
		void filterAll();
		void handleUserAction(boost::shared_ptr<UserRosterAction> action);
		TreeWidget *treeWidget_;
		TreeWidgetFactory *widgetFactory_;
		std::vector<RosterItem*> children_;
		std::vector<RosterItem*> items_;
		std::vector<RosterFilter*> filters_;
};
}

#endif
