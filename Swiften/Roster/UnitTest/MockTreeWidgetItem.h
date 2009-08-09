#ifndef SWIFTEN_MockTreeWidgetItem_H
#define SWIFTEN_MockTreeWidgetItem_H

#include "Swiften/Base/String.h"
#include "Swiften/Roster/TreeWidgetItem.h"

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {

class MockTreeWidgetItem : public TreeWidgetItem {
	public:
		virtual ~MockTreeWidgetItem() {};
		virtual void setText(const String&) {};
		virtual void setStatusText(const String&) {};
		virtual void setAvatarPath(const String&) {};
		virtual void setExpanded(bool) {};
		virtual void setTextColor(unsigned long) {};
		virtual void setBackgroundColor(unsigned long) {};
		virtual void show() {};
		virtual void hide() {};
};

}
#endif


