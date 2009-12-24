#pragma once

#include <QWidget>


namespace Swift {
	class QtTabbable : public QWidget {
		Q_OBJECT
		public:
			~QtTabbable();
			bool isWidgetSelected();
			virtual bool isWidgetAlerting() {return false;};
		protected:
			QtTabbable() : QWidget() {};

		signals:
			void titleUpdated();
			void windowClosing();
			void windowOpening();
			void wantsToActivate();
	};
}
