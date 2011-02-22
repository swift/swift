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
#include <QPainter>
#include <QBitmap>

#include "QtStatusWidget.h"
#include <Swift/QtUI/QtElidingLabel.h>
#include <Swift/QtUI/QtClickableLabel.h>
#include <Swift/QtUI/QtNameWidget.h>

namespace Swift {
QtRosterHeader::QtRosterHeader(QtSettingsProvider* settings, QWidget* parent) : QWidget(parent) {
	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->setSpacing(3);
	topLayout->setContentsMargins(4,4,4,4);
	setLayout(topLayout);
	setMinimumHeight(50);
	setMaximumHeight(50);

	avatarLabel_ = new QtClickableLabel(this);
	avatarLabel_->setMinimumSize(avatarSize_, avatarSize_);
	avatarLabel_->setMaximumSize(avatarSize_, avatarSize_);
	avatarLabel_->setAlignment(Qt::AlignCenter);
	setAvatar(":/icons/avatar.png");
	avatarLabel_->setScaledContents(false);
	topLayout->addWidget(avatarLabel_);
	connect(avatarLabel_, SIGNAL(clicked()), this, SIGNAL(onEditProfileRequest()));

	QVBoxLayout* rightLayout = new QVBoxLayout();
	rightLayout->setSpacing(4);
	rightLayout->setContentsMargins(4,0,0,0);
	topLayout->addLayout(rightLayout);

	nameWidget_ = new QtNameWidget(settings, this);
	connect(nameWidget_, SIGNAL(onChangeNickRequest()), this, SIGNAL(onEditProfileRequest()));
	rightLayout->addWidget(nameWidget_);


	statusWidget_ = new QtStatusWidget(this);
	connect(statusWidget_, SIGNAL(onChangeStatusRequest(StatusShow::Type, const QString&)), this, SLOT(handleChangeStatusRequest(StatusShow::Type, const QString&)));
	rightLayout->addWidget(statusWidget_);
	show();
}

void QtRosterHeader::handleChangeStatusRequest(StatusShow::Type type, const QString& text) {
	emit onChangeStatusRequest(type, text);
}

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
		//qDebug() << "Setting null avatar";
		avatar = QIcon(":/icons/avatar.png");
	} 

	// Apply a rounded rectangle mask
	// FIXME:
	// - We shouldn't go via a 128x128 pixmap
	// - Something tells me we can do this with clever composition mode +
	//   1 drawRectangle on the avatarPixmap, but i haven't figured it out yet.
	QPixmap avatarPixmap = avatar.pixmap(128, 128);
	QPixmap maskedAvatar(avatarPixmap.size());
	maskedAvatar.fill(QColor(0, 0, 0, 0));
	QPainter maskPainter(&maskedAvatar);
	maskPainter.setBrush(Qt::black);
	maskPainter.drawRoundedRect(maskedAvatar.rect(), 13, 13);
	maskPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	maskPainter.drawPixmap(0, 0, avatarPixmap);
	maskPainter.end();

	avatarLabel_->setPixmap(maskedAvatar.scaled(avatarSize_, avatarSize_, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void QtRosterHeader::setNick(const QString& nick) {
	nameWidget_->setNick(nick);
}

void QtRosterHeader::setJID(const QString& jid) {
	nameWidget_->setJID(jid);
}



const int QtRosterHeader::avatarSize_ = 40;

}
