#include "QtChatWindowFactory.h"

#include "QtChatTabs.h"
#include "QtChatWindow.h"
#include "QtSwiftUtil.h"
#include "QtTreeWidgetFactory.h"


namespace Swift {
QtChatWindowFactory::QtChatWindowFactory(QtTreeWidgetFactory *treeWidgetFactory) : treeWidgetFactory_(treeWidgetFactory) {
	tabs_ = new QtChatTabs();
}

ChatWindow* QtChatWindowFactory::createChatWindow(const JID &contact) {
	QtChatWindow *chatWindow = new QtChatWindow(P2QSTRING(contact.toString()), treeWidgetFactory_);
	tabs_->addTab(chatWindow);
	tabs_->show();
	//chatWindow->show();
	return chatWindow;
}

}
