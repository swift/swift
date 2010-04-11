/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once
#include <QTextEdit>

namespace Swift {
	class QtTextEdit : public QTextEdit {
		Q_OBJECT
	public:
		QtTextEdit(QWidget* parent = 0);
		virtual QSize sizeHint() const;
	signals:
		void returnPressed();
		void unhandledKeyPressEvent(QKeyEvent* event);
	protected:
		virtual void keyPressEvent(QKeyEvent* event);
	private slots:
		void handleTextChanged();
	};
}
