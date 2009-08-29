#pragma once

#include <QWidget>


namespace Swift {
	class QtTabbable : public QWidget {
		Q_OBJECT
		public:
			bool isWidgetSelected();
		protected:
			QtTabbable() : QWidget() {};

		signals:
			void titleUpdated();
			void windowClosing();
			void windowOpening();
			void wantsToActivate();
	};
}
