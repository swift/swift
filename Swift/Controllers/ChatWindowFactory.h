#ifndef SWIFTEN_CHATWINDOWFACTORY_H
#define SWIFTEN_CHATWINDOWFACTORY_H

#include "Swiften/JID/JID.h"

namespace Swift {
	class ChatWindow;

	class ChatWindowFactory {
		public:
			virtual ~ChatWindowFactory() {};
			/**
			 * Transfers ownership of result.
			 */
			virtual ChatWindow* createChatWindow(const JID &contact) = 0;

	};
}
#endif

