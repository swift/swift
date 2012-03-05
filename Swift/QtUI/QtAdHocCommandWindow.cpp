/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtAdHocCommandWindow.h>

#include <boost/bind.hpp>
#include <QBoxLayout>
#include <Swift/QtUI/QtFormWidget.h>
#include <Swiften/Elements/Command.h>

namespace Swift {
QtAdHocCommandWindow::QtAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command) : command_(command) {

	formWidget_ = NULL;

	setAttribute(Qt::WA_DeleteOnClose);
	command->onNextStageReceived.connect(boost::bind(&QtAdHocCommandWindow::handleNextStageReceived, this, _1));
	command->onError.connect(boost::bind(&QtAdHocCommandWindow::handleError, this, _1));
	command->start();

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(2);
	label_ = new QLabel(this);
	label_->setTextFormat(Qt::PlainText);
	layout->addWidget(label_);
	QWidget* formContainer = new QWidget(this);
	layout->addWidget(formContainer);
	formLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, formContainer);
	QWidget* buttonsWidget = new QWidget(this);
	layout->addWidget(buttonsWidget);

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
	show();
}

QtAdHocCommandWindow::~QtAdHocCommandWindow() {

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
	if (command->getForm()) {
		setForm(command->getForm());
	} else {
		setNoForm();
	}
	QString notes;
	foreach (Command::Note note, command->getNotes()) {
		if (!notes.isEmpty()) {
			notes += "\n";
			QString qNote(note.note.c_str());
			switch (note.type) {
				case Command::Note::Error: notes += tr("Error: %1").arg(qNote); break;
				case Command::Note::Warn: notes += tr("Warning: %1").arg(qNote); break;
				case Command::Note::Info: notes += qNote; break;
			}
		}
	}
	label_->setText(notes);
	setAvailableActions(command);
}

void QtAdHocCommandWindow::handleError(ErrorPayload::ref /*error*/) {
	nextButton_->setEnabled(false);
	backButton_->setEnabled(false);
	completeButton_->setEnabled(false);
	label_->setText(tr("Error executing command"));
}

void QtAdHocCommandWindow::setForm(Form::ref form) {
	delete formWidget_;
	formWidget_ = new QtFormWidget(form, this);
	formLayout_->addWidget(formWidget_);
}

void QtAdHocCommandWindow::setNoForm() {
	delete formWidget_;
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
