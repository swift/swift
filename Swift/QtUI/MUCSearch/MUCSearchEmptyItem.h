/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
