/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/EventWindowFactory.h"

namespace Swift {
	class QtMainWindowFactory;
	class QtEventWindowFactory : public EventWindowFactory {
		public:
			QtEventWindowFactory(QtMainWindowFactory* mainWindowFactory);
			virtual EventWindow* createEventWindow();
		private:
			QtMainWindowFactory* mainWindowFactory_;
	};
}

	
