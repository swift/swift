/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QDialog>

#include "ui_QtConnectionSettings.h"

#include <Swiften/Client/ClientOptions.h>

namespace Swift {
	class QtConnectionSettingsWindow : public QDialog {
			Q_OBJECT

		public:
			QtConnectionSettingsWindow(const ClientOptions& options);

			ClientOptions getOptions();

		private slots:
			void handleProxyTypeChanged(int);

		private:
			enum {
				NoProxy = 0,
				SystemProxy = 1,
				SOCKS5Proxy = 2,
				HTTPProxy = 3
			};
			Ui::QtConnectionSettings ui;
	};
}
