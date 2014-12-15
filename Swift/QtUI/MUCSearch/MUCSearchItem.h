/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QVariant>

namespace Swift {
	class MUCSearchItem {
		public:
			virtual ~MUCSearchItem() {}
			virtual QVariant data(int role) = 0;
	};
}
