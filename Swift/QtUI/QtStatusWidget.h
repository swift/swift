/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
			StatusShow::Type getSelectedStatusShow();
			void setStatusType(StatusShow::Type type);
		signals:
			void onChangeStatusRequest(StatusShow::Type showType);
		private slots:
			void handleTypeSelected(int index);
		private:
			QComboBox *types_;
	};
}

#endif


