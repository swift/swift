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
		protected:
			QtTabbable() : QWidget() {};

		signals:
			void titleUpdated();
			void windowClosing();
			void windowOpening();
			void wantsToActivate();
	};
}
