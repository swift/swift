#pragma once

#include <QWidget>


namespace Swift {
	class QtTabbable : public QWidget {
		Q_OBJECT
		protected:
			QtTabbable() : QWidget() {};

		signals:
			void titleUpdated();
			void windowClosing();
	};
}
