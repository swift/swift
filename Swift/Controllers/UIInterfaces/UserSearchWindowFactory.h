/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/UserSearchWindow.h"

namespace Swift {
	class UIEventStream;
	class UserSearchWindowFactory {
		public:
			virtual ~UserSearchWindowFactory() {};

			virtual UserSearchWindow* createUserSearchWindow(UIEventStream* eventStream) = 0;
	};
}
