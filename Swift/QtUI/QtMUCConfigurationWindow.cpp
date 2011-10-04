/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtMUCConfigurationWindow.h>

#include <boost/bind.hpp>
#include <QBoxLayout>
#include <QCloseEvent>
#include <Swift/QtUI/QtFormWidget.h>

namespace Swift {
QtMUCConfigurationWindow::QtMUCConfigurationWindow(Form::ref form) : closed_(false) {

	setAttribute(Qt::WA_DeleteOnClose);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(2);
	//QLabel* label = new QLabel(this);
	//label->setText(tr("Room configuration"));
	//layout->addWidget(label);

	formWidget_ = NULL;
	formWidget_ = new QtFormWidget(form, this);
	layout->addWidget(formWidget_);

	QWidget* buttonsWidget = new QWidget(this);
	layout->addWidget(buttonsWidget);

	QBoxLayout* buttonsLayout = new QBoxLayout(QBoxLayout::LeftToRight, buttonsWidget);
	cancelButton_ = new QPushButton(tr("Cancel"), buttonsWidget);
	buttonsLayout->addWidget(cancelButton_);
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(handleCancelClicked()));
	okButton_ = new QPushButton(tr("OK"), buttonsWidget);
	buttonsLayout->addWidget(okButton_);
	connect(okButton_, SIGNAL(clicked()), this, SLOT(handleOKClicked()));
	show();
}

QtMUCConfigurationWindow::~QtMUCConfigurationWindow() {

}

void QtMUCConfigurationWindow::closeEvent(QCloseEvent* event) {
	if (!closed_) {
		onFormCancelled();
	}
}

void QtMUCConfigurationWindow::handleCancelClicked() {
	close();
}

void QtMUCConfigurationWindow::handleOKClicked() {
	onFormComplete(formWidget_->getCompletedForm());
	closed_ = true;
	close();
}


}
