/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>

namespace Swift {
	class QtElidingLabel;
	class SettingsProvider;

	class QtNameWidget : public QWidget {
			Q_OBJECT

		public:
			QtNameWidget(SettingsProvider* settings, QWidget *parent);

			void setNick(const QString& text);
			void setJID(const QString& jid);

		signals:
			void onChangeNickRequest();

		private:
			void updateText();
			virtual void mousePressEvent(QMouseEvent* event);

		private:
			enum Mode {
				ShowNick,
				ShowJID,
			};

			SettingsProvider* settings;
			Mode mode;
			QtElidingLabel* textLabel;
			QString jid;
			QString nick;
	};
}

