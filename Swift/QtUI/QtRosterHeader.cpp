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
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0,0,0,0);
	setLayout(vLayout);

	toolBar_ = new QToolBar(this);
	vLayout->addWidget(toolBar_);

	statusWidget_ = new QtStatusWidget(this);
	toolBar_->addWidget(statusWidget_);
	statusWidget_->setMaximumWidth(60);
	connect(statusWidget_, SIGNAL(onChangeStatusRequest(StatusShow::Type)), this, SLOT(handleChangeStatusRequest(StatusShow::Type)));

	nameLabel_ = new QLabel(this);
	setName("Me");
	toolBar_->addWidget(nameLabel_);
	//nameLabel_->setMaximumWidth(width() - 5 - statusWidget_->width());
		
	expandedLayout_ = new QHBoxLayout();
	expandedLayout_->setContentsMargins(0,0,0,0);
	expandedLayout_->setSpacing(0);
	
	avatarLabel_ = new QLabel(this);
	setAvatar(":/icons/avatar.png");
	expandedLayout_->addWidget(avatarLabel_);
	
	statusEdit_ = new QTextEdit(this);
	expandedLayout_->addWidget(statusEdit_);
	statusEdit_->resize(statusEdit_->width(), 64);
	statusEdit_->setAcceptRichText(false);
	statusEdit_->setReadOnly(false);
	setStatusText("");

	vLayout->addLayout(expandedLayout_);
	expanded_ = false;
	avatarLabel_->hide();
	statusEdit_->hide();

	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
}

void QtRosterHeader::mousePressEvent(QMouseEvent* event) {
	if (nameLabel_->underMouse() || toolBar_->underMouse() && !statusWidget_->underMouse()) {
		toggleExpanded();
		event->accept();
	} else {
		event->ignore();
	}
	
}

void QtRosterHeader::toggleExpanded() {
	expanded_ = !expanded_;
	if (expanded_) {
		expandedLayout_->setContentsMargins(5,5,5,5);
		expandedLayout_->setSpacing(11);
		avatarLabel_->show();
		statusEdit_->show();
	} else {
		expandedLayout_->setContentsMargins(0,0,0,0);
		expandedLayout_->setSpacing(0);
		avatarLabel_->hide();
		statusEdit_->hide();
	}
}

void QtRosterHeader::handleChangeStatusRequest(StatusShow::Type type) {
	Q_UNUSED(type);
	emitStatus();
}

void QtRosterHeader::emitStatus() {
	emit onChangeStatusRequest(statusWidget_->getSelectedStatusShow(), statusEdit_->toPlainText());
}

void QtRosterHeader::setStatusText(const QString& statusMessage) {
	statusEdit_->setText(statusMessage);
}

void QtRosterHeader::setName(const QString& name) {
	name_ = name;
	resizeNameLabel();
}

void QtRosterHeader::resizeNameLabel() {	
	QString escapedName = name_;
	escapedName.replace("<","&lt;");
	nameLabel_->setText("<b>" + escapedName + "</b>");
	return;
	//FIXME: Make this not an infinite loop, so it can be continued.
	
	int reductionCount = 0;
	while (nameLabel_->sizeHint().width() + statusWidget_->width() + 30 > width()) {
		//qDebug() << nameLabel_->sizeHint().width() << " " << statusWidget_->width() << " " << width();
		reductionCount++;
		QString reducedName = name_;
		reducedName.remove(name_.length() - reductionCount, reductionCount);
		reducedName.replace("<","&lt;");
		nameLabel_->setText("<b>" + reducedName +  + "...</b>");
	//	qDebug() << "Shrunk " << escapedName << " down to " << reducedName;
	}
	nameLabel_->setToolTip(name_);
}

void QtRosterHeader::resizeEvent(QResizeEvent* event) {
	QWidget::resizeEvent(event);
	resizeNameLabel();
}

void QtRosterHeader::setAvatar(const QString& path) {
	avatarLabel_->setPixmap(QIcon(path).pixmap(64, 64));
}

QSize QtRosterHeader::sizeHint() const {
	return minimumSizeHint();
}

}
