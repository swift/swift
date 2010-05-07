/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "ui_QtBookmarkDetailWindow.h"

#include <boost/optional.hpp>

#include <QDialog>

#include "Swiften/MUC/MUCBookmark.h"

namespace Swift {
	class QtBookmarkDetailWindow : public QDialog, protected Ui::QtBookmarkDetailWindow {
		Q_OBJECT
		public:
			QtBookmarkDetailWindow(QWidget* parent = NULL);
			virtual bool commit() = 0;
			boost::optional<MUCBookmark> createBookmarkFromForm();

		public slots:
			void accept();
	};
}

