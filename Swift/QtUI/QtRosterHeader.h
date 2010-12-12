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
	class QtNameWidget;
	
	class QtRosterHeader : public QWidget {
		Q_OBJECT
	public:
		QtRosterHeader(QWidget* parent = NULL);
		void setAvatar(const QString& path);

		void setJID(const QString& jid);
		void setNick(const QString& nick);

		void setStatusText(const QString& statusMessage);
		void setStatusType(StatusShow::Type type);
		void setConnecting();
	signals:
		void onChangeStatusRequest(StatusShow::Type showType, const QString &statusMessage);
		void onChangeNickRequest(const QString &nick);

	private slots:
		void handleChangeStatusRequest(StatusShow::Type type, const QString &statusMessage);
	private:
		QString name_;
		QLabel* avatarLabel_;
		QtNameWidget* nameWidget_;
		QtTextEdit* statusEdit_;
		QToolBar* toolBar_;
		QtStatusWidget* statusWidget_;
		static const int avatarSize_;
	};
}
