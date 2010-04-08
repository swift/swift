/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtChatWindowFactory_H
#define SWIFT_QtChatWindowFactory_H

#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swiften/JID/JID.h"
#include "QtSettingsProvider.h"

#include <QObject>
#include <QSplitter>
namespace Swift {
	class QtTreeWidgetFactory;
	class QtChatTabs;
	class QtChatWindowFactory : public QObject, public ChatWindowFactory {
		Q_OBJECT
		public:
			QtChatWindowFactory(QtTreeWidgetFactory *treeWidgetFactory, QSplitter* splitter, QtSettingsProvider* settings, QtChatTabs* tabs);
			ChatWindow* createChatWindow(const JID &contact);
		private slots:
			void handleWindowGeometryChanged();
		private:
			QtTreeWidgetFactory* treeWidgetFactory_;
			QtSettingsProvider* settings_;
			QtChatTabs* tabs_;
	};
}

#endif
