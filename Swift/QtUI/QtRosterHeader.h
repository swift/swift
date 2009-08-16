#pragma once

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QTextEdit>
#include <QSize>
#include <QToolBar>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/StatusShow.h"

namespace Swift {
	class QtStatusWidget;
	
	class QtRosterHeader : public QWidget {
		Q_OBJECT
	public:
		QtRosterHeader(QWidget* parent = NULL);
		void setAvatar(const QString& path);
		void setName(const QString& name);
		void setStatusText(const QString& statusMessage);
		QSize sizeHint() const;
		void emitStatus();
	signals:
		void onChangeStatusRequest(StatusShow::Type showType, const QString &statusMessage);
	private slots:
		void handleChangeStatusRequest(StatusShow::Type type);
	protected:
		virtual void resizeEvent(QResizeEvent* event);
	private:
		void resizeNameLabel();
		QString name_;
		QLabel* avatarLabel_;
		QLabel* nameLabel_;
		QTextEdit* statusEdit_;
		QToolBar* toolBar_;
		QtStatusWidget* statusWidget_;
	};
}
