#ifndef SWIFT_QtChatWindowFactory_H
#define SWIFT_QtChatWindowFactory_H

#include "Swiften/Controllers/ChatWindowFactory.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class QtTreeWidgetFactory;
	class QtChatWindowFactory : public ChatWindowFactory {
		public:
			QtChatWindowFactory(QtTreeWidgetFactory *treeWidgetFactory);
			ChatWindow* createChatWindow(const JID &contact);
		private:
			QtTreeWidgetFactory *treeWidgetFactory_;
	};
}

#endif
