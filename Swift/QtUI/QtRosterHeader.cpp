/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtRosterHeader.h>

#include <QBitmap>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHelpEvent>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QSizePolicy>
#include <QToolTip>
#include <QVBoxLayout>

#include <qdebug.h>

#include <Swift/QtUI/QtClickableLabel.h>
#include <Swift/QtUI/QtElidingLabel.h>
#include <Swift/QtUI/QtNameWidget.h>
#include <Swift/QtUI/QtScaledAvatarCache.h>
#include <Swift/QtUI/QtStatusWidget.h>
#include <Swift/QtUI/Roster/RosterTooltip.h>

namespace Swift {
QtRosterHeader::QtRosterHeader(SettingsProvider* settings, StatusCache* statusCache, QWidget* parent) : QWidget(parent), statusEdit_(nullptr) {
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

    QHBoxLayout* nameAndSecurityLayout = new QHBoxLayout();
    nameAndSecurityLayout->setContentsMargins(4,0,0,0);

    nameWidget_ = new QtNameWidget(settings, this);
    connect(nameWidget_, SIGNAL(onChangeNickRequest()), this, SIGNAL(onEditProfileRequest()));
    nameAndSecurityLayout->addWidget(nameWidget_);

    securityInfoButton_ = new QToolButton(this);
    securityInfoButton_->setStyleSheet("QToolButton { border: none; } QToolButton:hover { border: 1px solid #bebebe; } QToolButton:pressed { border: 1px solid #757575; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #777777, stop: 1 #d4d4d4);}");
    //securityInfoButton_->setAutoRaise(true);
    securityInfoButton_->setIcon(QIcon(":/icons/lock.png"));
    securityInfoButton_->setToolTip(tr("Connection is secured"));
    connect(securityInfoButton_, SIGNAL(clicked()), this, SIGNAL(onShowCertificateInfo()));
    nameAndSecurityLayout->addWidget(securityInfoButton_);

    rightLayout->addLayout(nameAndSecurityLayout);

    statusWidget_ = new QtStatusWidget(statusCache, this);
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
    if (type == StatusShow::None) {
        nameWidget_->setOnline(false);
        disconnect(avatarLabel_, SIGNAL(clicked()), this, SIGNAL(onEditProfileRequest()));
    }
    else {
        nameWidget_->setOnline(true);
        connect(avatarLabel_, SIGNAL(clicked()), this, SIGNAL(onEditProfileRequest()), Qt::UniqueConnection);
    }
}

void QtRosterHeader::setConnecting() {
    statusWidget_->setConnecting();
}

void QtRosterHeader::setStreamEncryptionStatus(bool tlsInPlace) {
    securityInfoButton_->setVisible(tlsInPlace);
}

bool QtRosterHeader::event(QEvent* event) {
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QtScaledAvatarCache scaledAvatarCache(avatarSize_);
        QString text = RosterTooltip::buildDetailedTooltip(contact_.get(), &scaledAvatarCache);
        QToolTip::showText(helpEvent->globalPos(), text);
        return true;
    }
    return QWidget::event(event);
}

void QtRosterHeader::setAvatar(const QString& path) {
    QString scaledAvatarPath = QtScaledAvatarCache(avatarSize_).getScaledAvatarPath(path);
    QPixmap avatar;
    if (QFileInfo(scaledAvatarPath).exists()) {
        avatar.load(scaledAvatarPath);
    }
    else {
        avatar = QPixmap(":/icons/avatar.svg").scaled(avatarSize_, avatarSize_, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    avatarLabel_->setPixmap(avatar);
}

void QtRosterHeader::setNick(const QString& nick) {
    nameWidget_->setNick(nick);
}

void QtRosterHeader::setContactRosterItem(std::shared_ptr<ContactRosterItem> contact) {
    contact_ = contact;
}

void QtRosterHeader::setJID(const QString& jid) {
    nameWidget_->setJID(jid);
}



const int QtRosterHeader::avatarSize_ = 40;

}
