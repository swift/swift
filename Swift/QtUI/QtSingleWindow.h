/*
 * Copyright (c) 2010-2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QSplitter>

namespace Swift {
    class QtSettingsProvider;

    class QtSingleWindow : public QSplitter {
        Q_OBJECT
        public:
            QtSingleWindow(QtSettingsProvider* settings);
            virtual ~QtSingleWindow();
            void insertAtFront(QWidget* widget);
            void addWidget(QWidget* widget);
        protected:
            void resizeEvent(QResizeEvent*);
            void moveEvent(QMoveEvent*);
        private slots:
            void handleSplitterMoved(int, int);
            void handleTabsTitleChanged(const QString& title);
        private:
            void handleGeometryChanged();
            void restoreSplitters();

        private:

            QtSettingsProvider* settings_;
    };

}

