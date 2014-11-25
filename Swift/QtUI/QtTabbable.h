/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <QList>
#include <QWidget>

class QShortcut;

namespace Swift {
	class QtTabbable : public QWidget {
		Q_OBJECT
		public:
			enum AlertType {NoActivity, WaitingActivity, ImpendingActivity};
			virtual ~QtTabbable();
			bool isWidgetSelected();
			virtual AlertType getWidgetAlertState() {return NoActivity;}
			virtual int getCount() {return 0;}
			virtual std::string getID() const = 0;

		protected:
			QtTabbable();
			bool event(QEvent* event);

		signals:
			void titleUpdated();
			void countUpdated();
			void windowClosing();
			void windowOpening();
			void wantsToActivate();
			void requestPreviousTab();
			void requestNextTab();
			void requestActiveTab();
			void requestFlash();

		private:
			QList<QShortcut*> shortcuts;
	};
}
