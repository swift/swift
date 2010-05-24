/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QLineEdit>

namespace Swift {
	class QtLineEdit : public QLineEdit {
		Q_OBJECT
		public:
			QtLineEdit(QWidget* parent = NULL);
		signals:
			void escapePressed();
		protected:
			virtual void keyPressEvent(QKeyEvent* event);
	};
}
