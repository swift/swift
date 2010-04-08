/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_TreeWidgetFactory_H
#define SWIFTEN_TreeWidgetFactory_H

namespace Swift {

class TreeWidgetItem;
class TreeWidget;

class TreeWidgetFactory {
	public:
		virtual ~TreeWidgetFactory() {}
		virtual TreeWidget* createTreeWidget() = 0;
		virtual TreeWidgetItem* createTreeWidgetItem(TreeWidgetItem* item) = 0;
		virtual TreeWidgetItem* createTreeWidgetItem(TreeWidget* item) = 0;
};

}

#endif

