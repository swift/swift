/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/ProfileWindow.h>

namespace Swift {
	class ProfileWindowFactory {
		public:
			virtual ~ProfileWindowFactory() {};

			virtual ProfileWindow* createProfileWindow() = 0;
	};
}
