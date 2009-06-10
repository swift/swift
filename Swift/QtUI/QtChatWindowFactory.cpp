#include "QtChatWindowFactory.h"
#include "QtChatWindow.h"
#include "QtSwiftUtil.h"
#include "QtTreeWidgetFactory.h"

namespace Swift {
QtChatWindowFactory::QtChatWindowFactory(QtTreeWidgetFactory *treeWidgetFactory) : treeWidgetFactory_(treeWidgetFactory) {

}
ChatWindow* QtChatWindowFactory::createChatWindow(const JID &contact) {
	QtChatWindow *chatWindow = new QtChatWindow(P2QSTRING(contact.toString()), treeWidgetFactory_);
	chatWindow->show();
	return chatWindow;
}

}
