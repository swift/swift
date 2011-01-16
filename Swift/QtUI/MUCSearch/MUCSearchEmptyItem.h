/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/QtUI/MUCSearch/MUCSearchItem.h>

namespace Swift {
	class MUCSearchServiceItem;

	class MUCSearchEmptyItem : public MUCSearchItem {
		public:
			MUCSearchEmptyItem(MUCSearchServiceItem* parent);

			MUCSearchServiceItem* getParent();

			QVariant data(int role);

		private:
			MUCSearchServiceItem* parent;
	};
}
