/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/MUCSearchWindow.h"

namespace Swift {
	class UIEventStream;
	class MUCSearchWindowFactory {
		public:
			virtual ~MUCSearchWindowFactory() {};

			virtual MUCSearchWindow* createMUCSearchWindow() = 0;
	};
}
