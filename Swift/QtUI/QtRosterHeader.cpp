/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtRosterHeader.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QSizePolicy>
#include <qdebug.h>
#include <QMouseEvent>

#include "QtStatusWidget.h"
#include <Swift/QtUI/QtElidingLabel.h>
#include <Swift/QtUI/QtNameWidget.h>

namespace Swift {
QtRosterHeader::QtRosterHeader(QWidget* parent) : QWidget(parent) {
	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->setSpacing(0);
	topLayout->setContentsMargins(4,4,4,4);
	setLayout(topLayout);
	setMinimumHeight(50);
	setMaximumHeight(50);

	avatarLabel_ = new QLabel(this);
	avatarLabel_->setMinimumSize(avatarSize_, avatarSize_);
	avatarLabel_->setMaximumSize(avatarSize_, avatarSize_);
	avatarLabel_->setAlignment(Qt::AlignCenter);
	setAvatar(":/icons/avatar.png");
	avatarLabel_->setScaledContents(false);
	topLayout->addWidget(avatarLabel_);

	QVBoxLayout* rightLayout = new QVBoxLayout();
	rightLayout->setSpacing(4);
	rightLayout->setContentsMargins(4,0,0,0);
	topLayout->addLayout(rightLayout);

	nameWidget_ = new QtNameWidget(this);
	rightLayout->addWidget(nameWidget_);


	statusWidget_ = new QtStatusWidget(this);
	connect(statusWidget_, SIGNAL(onChangeStatusRequest(StatusShow::Type, const QString&)), this, SLOT(handleChangeStatusRequest(StatusShow::Type, const QString&)));
	rightLayout->addWidget(statusWidget_);
	show();
	//statusWidget_->setMaximumWidth(60);

	// statusEdit_ = new QtTextEdit(this);
	// expandedLayout_->addWidget(statusEdit_);
	// statusEdit_->resize(statusEdit_->width(), 64);
	// statusEdit_->setAcceptRichText(false);
	// statusEdit_->setReadOnly(false);
	// setStatusText("");
	// connect(statusEdit_, SIGNAL(returnPressed()), this, SLOT(emitStatus()));

	//setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
}

// void QtRosterHeader::mousePressEvent(QMouseEvent* event) {
// 	if (nameLabel_->underMouse() || (toolBar_->underMouse() && !statusWidget_->underMouse())) {
// 		toggleExpanded();
// 		event->accept();
// 	} else {
// 		event->ignore();
// 	}
	
// }


void QtRosterHeader::handleChangeStatusRequest(StatusShow::Type type, const QString& text) {
	emit onChangeStatusRequest(type, text);
}

//void QtRosterHeader::emitStatus() {
//	emit onChangeStatusRequest(statusWidget_->getSelectedStatusShow(), statusEdit_->toPlainText());
//}

void QtRosterHeader::setStatusText(const QString& statusMessage) {
	statusWidget_->setStatusText(statusMessage);
}

void QtRosterHeader::setStatusType(StatusShow::Type type) {
	statusWidget_->setStatusType(type);
}

void QtRosterHeader::setConnecting() {
	statusWidget_->setConnecting();
}

void QtRosterHeader::setAvatar(const QString& path) {
	QIcon avatar(path);
	if (avatar.isNull()) {
		qDebug() << "Setting null avatar";
		avatar = QIcon(":/icons/avatar.png");
	} 
	avatarLabel_->setPixmap(avatar.pixmap(avatarSize_, avatarSize_));
}

void QtRosterHeader::setNick(const QString& nick) {
	nameWidget_->setNick(nick);
}

void QtRosterHeader::setJID(const QString& jid) {
	nameWidget_->setJID(jid);
}



const int QtRosterHeader::avatarSize_ = 40;

}
