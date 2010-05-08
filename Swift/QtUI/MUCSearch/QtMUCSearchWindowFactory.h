/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/MUCSearchWindowFactory.h"
#include "Swift/QtUI/MUCSearch/QtMUCSearchWindow.h"

namespace Swift {
	class UIEventStream;
	class QtMUCSearchWindowFactory : public MUCSearchWindowFactory {
		public:
			virtual ~QtMUCSearchWindowFactory() {};

			MUCSearchWindow* createMUCSearchWindow(UIEventStream* eventStream) {
				return new QtMUCSearchWindow(eventStream);
			};
	};
}
