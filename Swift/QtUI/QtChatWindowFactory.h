/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <QMenu>
#include <QObject>
#include <QSplitter>
#include <QVBoxLayout>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>

#include <SwifTools/EmojiMapper.h>

#include <Swift/QtUI/QtEmojisSelector.h>
#include <Swift/QtUI/QtSettingsProvider.h>

namespace Swift {
    class QtChatTabsBase;
    class QtChatTheme;
    class UIEventStream;
    class QtUIPreferences;
    class QtSingleWindow;
    class QtChatWindowFactory : public QObject, public ChatWindowFactory {
        Q_OBJECT
        public:
            QtChatWindowFactory(QtSingleWindow* splitter, SettingsProvider* settings, QtSettingsProvider* qtSettings, QtChatTabsBase* tabs, const QString& themePath, const std::map<std::string, std::string>& emoticonsMap);
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
            QtChatTabsBase* tabs_;
            QtChatTheme* theme_;
            std::map<std::string, std::string> emoticonsMap_;
    };
}
