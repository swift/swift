#ifndef SWIFTEN_TreeWidgetItem_H
#define SWIFTEN_TreeWidgetItem_H

#include "Swiften/Base/String.h"
#include "Swiften/Roster/UserRosterAction.h"

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {

class TreeWidgetItem {
	public:
		virtual ~TreeWidgetItem() {}
		virtual void setText(const String& text) = 0;
		virtual void setStatusText(const String& text) = 0;
		virtual void setAvatarPath(const String& path) = 0;
		virtual void setExpanded(bool b) = 0;
		virtual void setTextColor(unsigned long color) = 0;
		virtual void setBackgroundColor(unsigned long color) = 0;
		boost::signal<void (boost::shared_ptr<UserRosterAction>)> onUserAction;
		virtual void show() = 0;
		virtual void hide() = 0;
		void performUserAction(boost::shared_ptr<UserRosterAction> action) {
			action->setTreeWidgetItem(this);
			onUserAction(action);
		}
};

}
#endif

