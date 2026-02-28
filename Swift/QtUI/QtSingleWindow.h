/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QListWidget>
#include <QSplitter>
#include <QStackedWidget>

#include <Swift/QtUI/ServerList/ServerListModel.h>

namespace Swift {
    class QtChatTabs;
    class QtLoginWindow;
    class QtSettingsProvider;
    class QtServerListView;
    class ServerListModel;

    class QtSingleWindow : public QSplitter {
        Q_OBJECT
        public:
            QtSingleWindow(QtSettingsProvider* settings);
            virtual ~QtSingleWindow();
            void addAccount(QtLoginWindow* widget, QtChatTabs* tabs);

        signals:
            void wantsToAddAccount();

        protected:
            void resizeEvent(QResizeEvent*);
            void moveEvent(QMoveEvent*);
        private slots:
            void handleSplitterMoved();
            void handleTabsTitleChanged(const QString& title);
            void handleListItemClicked(const QModelIndex&);
        private:
            void handleGeometryChanged();
            void restoreSplitters();

        private:

            QtSettingsProvider* settings_;
            SwiftAccountData accountData_;
            QtServerListView* serverList_;
            ServerListModel* serverListModel_;
            QStackedWidget* loginWindows_;
            QStackedWidget* tabs_;
    };

}

