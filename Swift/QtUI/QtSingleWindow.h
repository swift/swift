/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QListWidget>
#include <QSplitter>
#include <QStackedWidget>


namespace Swift {
    class QtChatTabs;
    class QtLoginWindow;
    class QtSettingsProvider;

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
            void handleListItemClicked(QListWidgetItem*);
        private:
            void handleGeometryChanged();
            void restoreSplitters();

        private:

            QtSettingsProvider* settings_;
            QListWidget* list_;
            QStackedWidget* loginWindows_;
            QStackedWidget* tabs_;
    };

}

