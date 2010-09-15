/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>

#include "SwifTools/Notifier/Win32NotifierWindow.h"

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
				return winId();
			}
	};
}
