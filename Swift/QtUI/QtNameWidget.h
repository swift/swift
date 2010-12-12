/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>

class QLabel;
class QStackedWidget;

namespace Swift {
	class QtElidingLabel;
	class QtLineEdit;

	class QtNameWidget : public QWidget {
			Q_OBJECT

		public:
			QtNameWidget(QWidget *parent);

			void setNick(const QString& text);
			void setJID(const QString& jid);

		signals:
			/**
			 * When this is emitted, the widget is disabled until
			 * setNick() is called.
			 */
			void onChangeNickRequest(const QString& nick);

		private slots:
			void handleEditComplete();
			void handleEditCancelled();
			virtual void mouseDoubleClickEvent(QMouseEvent* event);

		private:
			void updateText();

		private:
			QStackedWidget* stack;
			QtElidingLabel* textLabel;
			QtLineEdit* nickEdit;
			QString jid;
			QString nick;
	};
}

