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
	class QtChatTheme;
	class UIEventStream;
	class QtUIPreferences;
	class QtChatWindowFactory : public QObject, public ChatWindowFactory {
		Q_OBJECT
		public:
			QtChatWindowFactory(QSplitter* splitter, SettingsProvider* settings, QtSettingsProvider* qtSettings, QtChatTabs* tabs, const QString& themePath);
			~QtChatWindowFactory();
			ChatWindow* createChatWindow(const JID &contact, UIEventStream* eventStream);
		signals:
			void changeSplitterState(QByteArray);
		private slots:
			void handleWindowGeometryChanged();
			void handleSplitterMoved();
		private:
			QString themePath_;
			SettingsProvider* settings_;
			QtSettingsProvider* qtOnlySettings_;
			QtChatTabs* tabs_;
			QtChatTheme* theme_;
	};
}

