/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>


namespace Swift {
	class QtTabbable : public QWidget {
		Q_OBJECT
		public:
			enum AlertType {NoActivity, WaitingActivity, ImpendingActivity};
			~QtTabbable();
			bool isWidgetSelected();
			virtual AlertType getWidgetAlertState() {return NoActivity;};
			virtual int getCount() {return 0;}
		protected:
			QtTabbable() : QWidget() {};

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
	};
}
