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

	
