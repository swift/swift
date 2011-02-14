/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_LoginWindowFactory_H
#define SWIFTEN_LoginWindowFactory_H



namespace Swift {
	class LoginWindow;
	
	class UIEventStream;
		
	class LoginWindowFactory {
		public:
			virtual ~LoginWindowFactory() {};

			/**
			 * Transfers ownership of result.
			 */
			virtual LoginWindow* createLoginWindow(UIEventStream* uiEventStream) = 0;

	};
}
#endif

