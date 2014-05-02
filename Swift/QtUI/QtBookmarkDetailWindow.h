/*
 * Copyright (c) 2010-2014 Kevin Smith
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

		protected:
			void createFormFromBookmark(const MUCBookmark& bookmark);

		public slots:
			void accept();
	};
}

