/*
 * Copyright (c) 2010-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <QBoxLayout>
#include <Swift/QtUI/QtAdHocCommandWindow.h>
#include <Swift/QtUI/QtFormWidget.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swiften/Base/format.h>
#include <Swiften/Elements/Command.h>

const int FormLayoutIndex = 1;

namespace Swift {
QtAdHocCommandWindow::QtAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command) : command_(command) {
	formWidget_ = NULL;

	setAttribute(Qt::WA_DeleteOnClose);
	command->onNextStageReceived.connect(boost::bind(&QtAdHocCommandWindow::handleNextStageReceived, this, _1));
	command->onError.connect(boost::bind(&QtAdHocCommandWindow::handleError, this, _1));
	command->start();

	layout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout_->setContentsMargins(0,0,0,0);
	layout_->setSpacing(2);
	label_ = new QLabel(this);
	label_->setTextFormat(Qt::PlainText);
	layout_->addWidget(label_);

	errorLabel_ = new QLabel(this);
	errorLabel_->setText(QString("<b>%1</b>").arg(tr("Unable to complete the command because you have been disconnected")));
	errorLabel_->setVisible(false);
	errorLabel_->setFrameStyle(QFrame::Box|QFrame::Sunken);
	layout_->addWidget(errorLabel_);

	QWidget* buttonsWidget = new QWidget(this);
	layout_->addWidget(buttonsWidget);

	QBoxLayout* buttonsLayout = new QBoxLayout(QBoxLayout::LeftToRight, buttonsWidget);
	cancelButton_ = new QPushButton(tr("Cancel"), buttonsWidget);
	buttonsLayout->addWidget(cancelButton_);
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(handleCancelClicked()));
	backButton_ = new QPushButton(tr("Back"), buttonsWidget);
	buttonsLayout->addWidget(backButton_);
	connect(backButton_, SIGNAL(clicked()), this, SLOT(handlePrevClicked()));
	nextButton_ = new QPushButton(tr("Next"), buttonsWidget);
	buttonsLayout->addWidget(nextButton_);
	connect(nextButton_, SIGNAL(clicked()), this, SLOT(handleNextClicked()));
	completeButton_ = new QPushButton(tr("Complete"), buttonsWidget);
	buttonsLayout->addWidget(completeButton_);
	connect(completeButton_, SIGNAL(clicked()), this, SLOT(handleCompleteClicked()));
	nextButton_->setEnabled(false);
	backButton_->setEnabled(false);
	completeButton_->setEnabled(false);

	actions_[Command::Next] = nextButton_;
	actions_[Command::Prev] = backButton_;
	actions_[Command::Complete] = completeButton_;
	actions_[Command::Cancel] = cancelButton_;
}

QtAdHocCommandWindow::~QtAdHocCommandWindow() {
}

void QtAdHocCommandWindow::setOnline(bool online) {
	if (!online) {
		nextButton_->setEnabled(false);
		backButton_->setEnabled(false);
		completeButton_->setEnabled(false);
		errorLabel_->setVisible(true);
	}
}

void QtAdHocCommandWindow::closeEvent(QCloseEvent*) {
	onClosing();
}

void QtAdHocCommandWindow::handleCancelClicked() {
	command_->cancel();
	close();
}

void QtAdHocCommandWindow::handlePrevClicked() {
	command_->goBack();
}

void QtAdHocCommandWindow::handleNextClicked() {
	command_->goNext(formWidget_ ? formWidget_->getCompletedForm() : Form::ref());
}

void QtAdHocCommandWindow::handleCompleteClicked() {
	command_->complete(formWidget_ ? formWidget_->getCompletedForm() : Form::ref());
}

void QtAdHocCommandWindow::handleNextStageReceived(Command::ref command) {
	QString notes;
	foreach (Command::Note note, command->getNotes()) {
		if (!notes.isEmpty()) {
			notes += "\n";
		}
		QString qNote(P2QSTRING(note.note));
		switch (note.type) {
			case Command::Note::Error: notes += tr("Error: %1").arg(qNote); break;
			case Command::Note::Warn: notes += tr("Warning: %1").arg(qNote); break;
			case Command::Note::Info: notes += qNote; break;
		}
	}
	label_->setText(notes);
	if (command->getForm()) {
		setForm(command->getForm());
	} else {
		setNoForm(notes.isEmpty());
	}
	setAvailableActions(command);
}

void QtAdHocCommandWindow::handleError(ErrorPayload::ref /*error*/) {
	nextButton_->setEnabled(false);
	backButton_->setEnabled(false);
	completeButton_->setEnabled(false);
	label_->setText(tr("Error executing command"));
}

void QtAdHocCommandWindow::setForm(Form::ref form) {
	form_ = form;
	delete formWidget_;
	formWidget_ = new QtFormWidget(form, this);
	layout_->insertWidget(FormLayoutIndex, formWidget_);
	show();
}

void QtAdHocCommandWindow::setNoForm(bool andHide) {
	form_.reset();
	delete formWidget_;
	formWidget_ = NULL;
	resize(minimumSize());
	setVisible(!andHide);
}

typedef std::pair<Command::Action, QPushButton*> ActionButton;

void QtAdHocCommandWindow::setAvailableActions(Command::ref /*commandResult*/) {
	foreach (ActionButton pair, actions_) {
		OutgoingAdHocCommandSession::ActionState state = command_->getActionState(pair.first);
		if (state & OutgoingAdHocCommandSession::Present) {
			pair.second->show();
		}
		else {
			pair.second->hide();
		}
		if (state & OutgoingAdHocCommandSession::Enabled) {
			pair.second->setEnabled(true);
		}
		else {
			pair.second->setEnabled(false);
		}
	}
}

}
