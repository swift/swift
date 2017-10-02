/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWidget>

#include <SwifTools/Notifier/Win32NotifierWindow.h>

namespace Swift {
    class QtWin32NotifierWindow : public QWidget, public Win32NotifierWindow {
        public:
            QtWin32NotifierWindow(QWidget* parent = NULL) {
                setVisible(false);
            }

            bool winEvent (MSG* message, long* result ) {
                onMessageReceived(message);
                return false;
            }

            virtual HWND getID() const {
                return (HWND) winId();
            }
    };
}
