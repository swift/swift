/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ContactEditWindow.h>

namespace Swift {
	class ContactEditWindowFactory {
		public:
			virtual ~ContactEditWindowFactory() {};

			virtual ContactEditWindow* createContactEditWindow() = 0;
	};
}
