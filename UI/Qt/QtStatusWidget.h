#ifndef SWIFT_QtStatusWidget_H
#define SWIFT_QtStatusWidget_H

#include "Swiften/Elements/StatusShow.h"

#include <QWidget>

class QComboBox;
class QLineEdit;

namespace Swift {
	class QtStatusWidget : public QWidget {
		Q_OBJECT
		public:
			QtStatusWidget(QWidget *parent);
		signals:
			void onChangeStatusRequest(StatusShow::Type showType, const QString &statusMessage);
		private slots:
			void handleTypeSelected(int index);
		private:
			QComboBox *types_;
			QLineEdit *message_;
	};
}

#endif


