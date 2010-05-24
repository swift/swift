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

namespace Swift {
QtRosterHeader::QtRosterHeader(QWidget* parent) : QWidget(parent) {
	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->setSpacing(0);
	topLayout->setContentsMargins(4,4,4,4);
	setLayout(topLayout);
	setMinimumHeight(50);
	setMaximumHeight(50);

	avatarLabel_ = new QLabel(this);
	avatarLabel_->setMinimumSize(40, 40);
	avatarLabel_->setMaximumSize(40, 40);
	setAvatar(":/icons/avatar.png");
	avatarLabel_->setScaledContents(true);
	topLayout->addWidget(avatarLabel_);

	QVBoxLayout* rightLayout = new QVBoxLayout();
	rightLayout->setSpacing(4);
	rightLayout->setContentsMargins(4,0,0,0);
	topLayout->addLayout(rightLayout);

	nameLabel_ = new QLabel(this);
	setName("Me");
	rightLayout->addWidget(nameLabel_);


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

void QtRosterHeader::setName(const QString& name) {
	name_ = name;
	QString escapedName = name_;
	escapedName.replace("<","&lt;");
 	nameLabel_->setText("<b>" + escapedName + "</b>");
//	resizeNameLabel();
}

// void QtRosterHeader::resizeNameLabel() {	
// 	QString escapedName = name_;
// 	escapedName.replace("<","&lt;");
// 	nameLabel_->setText("<b>" + escapedName + "</b>");
// 	return;
// 	//FIXME: Make this not an infinite loop, so it can be continued.
	
// 	int reductionCount = 0;
// 	while (nameLabel_->sizeHint().width() + statusWidget_->width() + 30 > width()) {
// 		//qDebug() << nameLabel_->sizeHint().width() << " " << statusWidget_->width() << " " << width();
// 		reductionCount++;
// 		QString reducedName = name_;
// 		reducedName.remove(name_.length() - reductionCount, reductionCount);
// 		reducedName.replace("<","&lt;");
// 		nameLabel_->setText("<b>" + reducedName +  + "...</b>");
// 	//	qDebug() << "Shrunk " << escapedName << " down to " << reducedName;
// 	}
// 	nameLabel_->setToolTip(name_);
// }

//void QtRosterHeader::resizeEvent(QResizeEvent* event) {
//	QWidget::resizeEvent(event);
//	resizeNameLabel();
//}

void QtRosterHeader::setAvatar(const QString& path) {
	QIcon avatar(path);
	if (avatar.isNull()) {
		qDebug() << "Setting null avatar";
		avatar = QIcon(":/icons/avatar.png");
	} 
	avatarLabel_->setPixmap(avatar.pixmap(64, 64));
}

//QSize QtRosterHeader::sizeHint() const {
//	return minimumSizeHint();
//}

}
