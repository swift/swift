#pragma once

#include <QWidget>

namespace Swift {
	class QtAboutWidget : public QWidget {
		Q_OBJECT
		public:
			static QtAboutWidget* instance();

		private:
			QtAboutWidget();
			static QtAboutWidget* instance_;
	};
}