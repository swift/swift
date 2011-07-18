/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/JoinMUCWindow.h>

namespace Swift {
	class UIEventStream;
	class JoinMUCWindowFactory {
		public:
			virtual ~JoinMUCWindowFactory() {};

			virtual JoinMUCWindow* createJoinMUCWindow(UIEventStream* uiEventStream) = 0;
	};
}
