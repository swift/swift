#ifndef SWIFTEN_MainWindowFactory_H
#define SWIFTEN_MainWindowFactory_H

#include "Swiften/JID/JID.h"

namespace Swift {
	class MainWindow;

	class MainWindowFactory {
		public:
			virtual ~MainWindowFactory() {};
			/**
			 * Transfers ownership of result.
			 */
			virtual MainWindow* createMainWindow() = 0;

	};
}
#endif

