/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_CHATWINDOWFACTORY_H
#define SWIFTEN_CHATWINDOWFACTORY_H

#include "Swiften/JID/JID.h"

namespace Swift {
	class ChatWindow;
	class UIEventStream;
	class ChatWindowFactory {
		public:
			virtual ~ChatWindowFactory() {};
			/**
			 * Transfers ownership of result.
			 */
			virtual ChatWindow* createChatWindow(const JID &contact, UIEventStream* eventStream) = 0;

	};
}
#endif

