#include "Swift/QtUI/QtSubscriptionRequestWindow.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {
QtSubscriptionRequestWindow::QtSubscriptionRequestWindow(boost::shared_ptr<SubscriptionRequestEvent> event, QWidget* parent) : QDialog(parent), event_(event) {
	QString text = P2QSTRING(event->getJID().toString()) + " would like to add you to their roster.\n Would you like to add them to your roster and share your status when you're online? \n\nIf you choose to defer this choice, you'll be asked again when you next login.";
	QVBoxLayout* layout = new QVBoxLayout();
	QLabel* label = new QLabel(text, this);
	layout->addWidget(label);

	if (event_->getConcluded()) {
		QLabel* doneLabel = new QLabel("You have already replied to this request");
		QPushButton* okButton = new QPushButton("OK", this);
		connect(okButton, SIGNAL(clicked()), this, SLOT(handleDefer()));
		layout->addWidget(doneLabel);
		layout->addWidget(okButton);
	} else {
		QPushButton* yesButton = new QPushButton("Yes", this);
		connect(yesButton, SIGNAL(clicked()), this, SLOT(handleYes()));
		QPushButton* noButton = new QPushButton("No", this);
		connect(noButton, SIGNAL(clicked()), this, SLOT(handleNo()));
		QPushButton* deferButton = new QPushButton("Defer", this);
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
