#include "QtChatWindowFactory.h"

#include "QtChatTabs.h"
#include "QtChatWindow.h"
#include "QtSwiftUtil.h"
#include "Roster/QtTreeWidgetFactory.h"


namespace Swift {
QtChatWindowFactory::QtChatWindowFactory(QtTreeWidgetFactory *treeWidgetFactory, QSplitter* splitter) : treeWidgetFactory_(treeWidgetFactory) {
	tabs_ = new QtChatTabs();
	if (splitter) {
		splitter->addWidget(tabs_);
	}
}

ChatWindow* QtChatWindowFactory::createChatWindow(const JID &contact) {
	QtChatWindow *chatWindow = new QtChatWindow(P2QSTRING(contact.toString()), treeWidgetFactory_);
	tabs_->addTab(chatWindow);
	if (!tabs_->isVisible()) {
		tabs_->showMinimized();
		//tabs_->minimise();
	}
	//chatWindow->show();
	return chatWindow;
}

}
