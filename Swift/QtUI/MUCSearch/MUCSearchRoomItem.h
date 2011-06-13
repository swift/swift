/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/QtUI/MUCSearch/MUCSearchItem.h"

namespace Swift {
	class MUCSearchServiceItem;
	class MUCSearchRoomItem : public MUCSearchItem {
		public:
			MUCSearchRoomItem(const QString& node, MUCSearchServiceItem* parent);
			MUCSearchServiceItem* getParent();
			QVariant data(int role);
			QString getNode() const {return node_;}
		private:
			MUCSearchServiceItem* parent_;
			QString node_;
	};
}
