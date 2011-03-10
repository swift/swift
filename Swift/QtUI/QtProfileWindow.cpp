/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtProfileWindow.h"

#include <QImage>
#include <QPixmap>
#include <QSizePolicy>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMovie>

#include "QtSwiftUtil.h"
#include "QtAvatarWidget.h"

namespace Swift {

QtProfileWindow::QtProfileWindow() {
	setWindowTitle(tr("Edit Profile"));

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
	setSizePolicy(sizePolicy);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(10, 10, 10, 10);

	QHBoxLayout* topLayout = new QHBoxLayout();

	avatar = new QtAvatarWidget(this);
	topLayout->addWidget(avatar);

	QVBoxLayout* fieldsLayout = new QVBoxLayout();

	QHBoxLayout* horizontalLayout_2 = new QHBoxLayout();
	nicknameLabel = new QLabel(tr("Nickname:"), this);
	horizontalLayout_2->addWidget(nicknameLabel);
	nickname = new QLineEdit(this);
	horizontalLayout_2->addWidget(nickname);

	fieldsLayout->addLayout(horizontalLayout_2);

	errorLabel = new QLabel(this);
	errorLabel->setAlignment(Qt::AlignHCenter);
	fieldsLayout->addWidget(errorLabel);

	fieldsLayout->addItem(new QSpacerItem(198, 17, QSizePolicy::Minimum, QSizePolicy::Expanding));
	topLayout->addLayout(fieldsLayout);

	layout->addLayout(topLayout);

	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	horizontalLayout->setContentsMargins(0, 0, 0, 0);
	horizontalLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	throbberLabel = new QLabel(this);
	throbberLabel->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), this));
	horizontalLayout->addWidget(throbberLabel);

	saveButton = new QPushButton(tr("Save"), this);
	saveButton->setDefault( true );
	connect(saveButton, SIGNAL(clicked()), SLOT(handleSave()));
	horizontalLayout->addWidget(saveButton);

	fieldsLayout->addLayout(horizontalLayout);

	resize(360, 120);
}

void QtProfileWindow::setVCard(Swift::VCard::ref vcard) {
	this->vcard = vcard;
	nickname->setText(P2QSTRING(vcard->getNickname()));
	avatar->setAvatar(vcard->getPhoto(), vcard->getPhotoType());
}

void QtProfileWindow::setEnabled(bool b) {
	nickname->setEnabled(b);
	nicknameLabel->setEnabled(b);
	avatar->setEnabled(b);
	saveButton->setEnabled(b);
}

void QtProfileWindow::setProcessing(bool processing) {
	if (processing) {
		throbberLabel->movie()->start();
		throbberLabel->show();
	}
	else {
		throbberLabel->hide();
		throbberLabel->movie()->stop();
	}
}

void QtProfileWindow::show() {
	QWidget::show();
	QWidget::activateWindow();
}

void QtProfileWindow::hideEvent(QHideEvent* event) {
	QWidget::hideEvent(event);
}

void QtProfileWindow::hide() {
	QWidget::hide();
}

void QtProfileWindow::handleSave() {
	assert(vcard);
	vcard->setNickname(Q2PSTRING(nickname->text()));
	vcard->setPhoto(avatar->getAvatarData());
	vcard->setPhotoType(avatar->getAvatarType());
	onVCardChangeRequest(vcard);
}

void QtProfileWindow::setError(const std::string& error) {
	if (!error.empty()) {
		errorLabel->setText("<font color='red'>" + P2QSTRING(error) + "</font>");
	}
	else {
		errorLabel->setText("");
	}
}



}
