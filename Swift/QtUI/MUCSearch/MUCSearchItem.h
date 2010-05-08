/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
