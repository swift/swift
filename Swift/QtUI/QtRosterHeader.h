/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QSize>
#include <QToolBar>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/StatusShow.h"

#include "QtTextEdit.h"

class QHBoxLayout;

namespace Swift {
	class QtStatusWidget;
	
	class QtRosterHeader : public QWidget {
		Q_OBJECT
	public:
		QtRosterHeader(QWidget* parent = NULL);
		void setAvatar(const QString& path);
		void setName(const QString& name);
		void setStatusText(const QString& statusMessage);
		void setStatusType(StatusShow::Type type);
//		QSize sizeHint() const;
//	public slots:
//		void emitStatus();
	signals:
		void onChangeStatusRequest(StatusShow::Type showType, const QString &statusMessage);
	private slots:
		void handleChangeStatusRequest(StatusShow::Type type, const QString &statusMessage);
//	protected:
//		virtual void resizeEvent(QResizeEvent* event);
//		virtual void mousePressEvent(QMouseEvent* event);
	private:
//		void resizeNameLabel();
//		void toggleExpanded();
		QString name_;
		QLabel* avatarLabel_;
#warning FIXME - replace QLabel with override to do elide
//http://lists.trolltech.com/pipermail/qt-interest/2010-January/018056.html
		QLabel* nameLabel_;
		QtTextEdit* statusEdit_;
		QToolBar* toolBar_;
		QtStatusWidget* statusWidget_;
		//QHBoxLayout* expandedLayout_;
		//bool expanded_;
	};
}
