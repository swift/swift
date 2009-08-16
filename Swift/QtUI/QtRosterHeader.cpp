#include "QtRosterHeader.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QSizePolicy>

#include "QtStatusWidget.h"

namespace Swift {
QtRosterHeader::QtRosterHeader(QWidget* parent) : QWidget(parent) {
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0,0,0,0);
	setLayout(vLayout);

	//QHBoxLayout* topLayout = new QHBoxLayout();
	//vLayout->addLayout(topLayout);
	
	toolBar_ = new QToolBar(this);
	vLayout->addWidget(toolBar_);

	statusWidget_ = new QtStatusWidget(this);
	toolBar_->addWidget(statusWidget_);
	statusWidget_->resize(50, statusWidget_->height());
	connect(statusWidget_, SIGNAL(onChangeStatusRequest(StatusShow::Type, const QString&)), this, SIGNAL(onChangeStatusRequest(StatusShow::Type, const QString&)));

	nameLabel_ = new QLabel(this);
	setName("Me");
	//topLayout->addWidget(nameLabel_);
	toolBar_->addWidget(nameLabel_);
		
	QHBoxLayout* expandedLayout = new QHBoxLayout();
	expandedLayout->setContentsMargins(5,5,5,5);
	expandedLayout->setSpacing(11);
	
	avatarLabel_ = new QLabel(this);
	setAvatar(":/icons/avatar.png");
	expandedLayout->addWidget(avatarLabel_);
	
	statusEdit_ = new QTextEdit(this);
	expandedLayout->addWidget(statusEdit_);
	statusEdit_->resize(statusEdit_->width(), 64);
	statusEdit_->setAcceptRichText(false);
	statusEdit_->setReadOnly(false);
	setStatusText("Could be here, could be away.");

	vLayout->addLayout(expandedLayout);

	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
}

void QtRosterHeader::setStatusText(const QString& statusMessage) {
	statusEdit_->setText(statusMessage);
}

void QtRosterHeader::setName(const QString& name) {
	QString escapedName = name;
	escapedName.replace("<","&lt;");
	nameLabel_->setText("<b>" + escapedName + "</b>");
}

void QtRosterHeader::setAvatar(const QString& path) {
	avatarLabel_->setPixmap(QIcon(path).pixmap(64, 64));
}

QSize QtRosterHeader::sizeHint() const {
	return minimumSizeHint();
}

}
