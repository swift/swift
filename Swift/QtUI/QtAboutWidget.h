/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QDialog>

namespace Swift {
	class QtAboutWidget : public QDialog {
			Q_OBJECT

		public:
			QtAboutWidget();

		private slots:
			void handleLicenseClicked();
	};
}
