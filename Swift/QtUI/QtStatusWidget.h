/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/StatusShow.h"

#include <QWidget>
#include <QMap>
#include <QIcon>

class QComboBox;
class QLabel;
class QStackedWidget;
class QListWidget;
class QListWidgetItem;
class QMovie;

namespace Swift {
	class QtLineEdit;
	class QtElidingLabel;
	class QtStatusWidget : public QWidget {
		Q_OBJECT
		public:
			QtStatusWidget(QWidget *parent);
			~QtStatusWidget();
			StatusShow::Type getSelectedStatusShow();
			void setStatusType(StatusShow::Type type);
			void setConnecting();
		signals:
			void onChangeStatusRequest(StatusShow::Type showType, const QString& text);
		public slots:
			void setStatusText(const QString& text);
		private slots:
			void generateList();
			void handleClicked();
			void handleEditComplete();
			void handleEditCancelled();
			void handleApplicationFocusChanged(QWidget* old, QWidget* now);
		protected slots:
			virtual void mousePressEvent(QMouseEvent* event);
			void handleItemClicked(QListWidgetItem* item);
			static QString getNoMessage();
		private:
			void viewMode();
			void setNewToolTip();
			//QComboBox *types_;
			QStackedWidget* stack_;
			QLabel* statusIcon_;
			QtElidingLabel* statusTextLabel_;
			QtLineEdit* statusEdit_;
			QString statusText_;
			QString newStatusText_;
			QMap<StatusShow::Type, QIcon> icons_;
			StatusShow::Type selectedStatusType_;
			bool isClicking_;
			QListWidget* menu_;
			QCursor editCursor_;
			QCursor viewCursor_;
			bool editing_;
			QMovie* connectingMovie_;
			bool connecting_;
			static const QString NO_MESSAGE;
	};
}

