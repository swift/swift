/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swiften/JID/JID.h"
#include "QtSettingsProvider.h"

#include <QObject>
#include <QSplitter>
namespace Swift {
	class QtChatTabs;
	class UIEventStream;
	class QtChatWindowFactory : public QObject, public ChatWindowFactory {
		Q_OBJECT
		public:
			QtChatWindowFactory(QSplitter* splitter, QtSettingsProvider* settings, QtChatTabs* tabs);
			ChatWindow* createChatWindow(const JID &contact, UIEventStream* eventStream);
		private slots:
			void handleWindowGeometryChanged();
		private:
			QtSettingsProvider* settings_;
			QtChatTabs* tabs_;
	};
}

