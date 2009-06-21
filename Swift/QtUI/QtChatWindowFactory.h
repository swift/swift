#ifndef SWIFT_QtChatWindowFactory_H
#define SWIFT_QtChatWindowFactory_H

#include "Swift/Controllers/ChatWindowFactory.h"
#include "Swiften/JID/JID.h"

#include <QSplitter>
namespace Swift {
	class QtTreeWidgetFactory;
	class QtChatTabs;
	class QtChatWindowFactory : public ChatWindowFactory {
		public:
			QtChatWindowFactory(QtTreeWidgetFactory *treeWidgetFactory, QSplitter* splitter);
			ChatWindow* createChatWindow(const JID &contact);
		private:
			QtTreeWidgetFactory* treeWidgetFactory_;
			QtChatTabs* tabs_;
	};
}

#endif
