/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/QtSubscriptionRequestWindow.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {
QtSubscriptionRequestWindow::QtSubscriptionRequestWindow(boost::shared_ptr<SubscriptionRequestEvent> event, QWidget* parent) : QDialog(parent), event_(event) {
	QString text = QString(tr("%1 would like to add you to their contact list.\n Would you like to add them to your contact list and share your status when you're online? \n\nIf you choose to defer this choice, you will be asked again when you next login.")).arg(event->getJID().toString().c_str());
	QVBoxLayout* layout = new QVBoxLayout();
	QLabel* label = new QLabel(text, this);
	layout->addWidget(label);

	if (event_->getConcluded()) {
		QLabel* doneLabel = new QLabel(tr("You have already replied to this request"));
		QPushButton* okButton = new QPushButton(tr("OK"), this);
		connect(okButton, SIGNAL(clicked()), this, SLOT(handleDefer()));
		layout->addWidget(doneLabel);
		layout->addWidget(okButton);
	} else {
		QPushButton* yesButton = new QPushButton(tr("Yes"), this);
		yesButton->setDefault(true);
		connect(yesButton, SIGNAL(clicked()), this, SLOT(handleYes()));
		QPushButton* noButton = new QPushButton(tr("No"), this);
		connect(noButton, SIGNAL(clicked()), this, SLOT(handleNo()));
		QPushButton* deferButton = new QPushButton(tr("Defer"), this);
		deferButton->setShortcut(QKeySequence(Qt::Key_Escape));
		connect(deferButton, SIGNAL(clicked()), this, SLOT(handleDefer()));

		QHBoxLayout* buttonLayout = new QHBoxLayout();
		buttonLayout->addWidget(yesButton);
		buttonLayout->addWidget(noButton);
		buttonLayout->addWidget(deferButton);
	
		layout->addLayout(buttonLayout);
	}

	setLayout(layout);
}

void QtSubscriptionRequestWindow::handleYes() {
	event_->accept();
	delete this;
}

void QtSubscriptionRequestWindow::handleNo() {
	event_->decline();
	delete this;
}

void QtSubscriptionRequestWindow::handleDefer() {
	event_->defer();
	delete this;
}

QtSubscriptionRequestWindow::~QtSubscriptionRequestWindow() {
	windows_.removeOne(this);
}

QtSubscriptionRequestWindow* QtSubscriptionRequestWindow::getWindow(boost::shared_ptr<SubscriptionRequestEvent> event, QWidget* parent) {
	foreach (QtSubscriptionRequestWindow* window, windows_) {
		if (window->getEvent() == event) {
			return window;
		}
	}
	QtSubscriptionRequestWindow* window = new QtSubscriptionRequestWindow(event, parent);
	windows_.append(window);
	return window;
}

boost::shared_ptr<SubscriptionRequestEvent> QtSubscriptionRequestWindow::getEvent() {
	return event_;
}

QList<QtSubscriptionRequestWindow*> QtSubscriptionRequestWindow::windows_;

}
