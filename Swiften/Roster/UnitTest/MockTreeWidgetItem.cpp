/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
#include "Swiften/Roster/UnitTest/MockTreeWidgetItem.h"

#include "Swiften/Roster/UnitTest/MockTreeWidgetFactory.h"

namespace Swift {

MockTreeWidgetItem::~MockTreeWidgetItem() {
	factory_->removeItem(this);
}

}



