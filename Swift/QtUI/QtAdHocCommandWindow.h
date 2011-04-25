/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>

#include <Swift/Controllers/UIInterfaces/AdHocCommandWindow.h>
#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

class QBoxLayout;

namespace Swift {
	class QtFormWidget;
	class QtAdHocCommandWindow : public QWidget, public AdHocCommandWindow {
		Q_OBJECT
		public:
			QtAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command);
			virtual ~QtAdHocCommandWindow();
		private:
			void handleNextStageReceived(Command::ref command);
			void handleError(ErrorPayload::ref error);
			void setForm(Form::ref);
			void setNoForm();
			void setAvailableActions(Command::ref commandResult);
		private slots:
			void handleCancelClicked();
			void handlePrevClicked();
			void handleNextClicked();
			void handleCompleteClicked();
		private:
			boost::shared_ptr<OutgoingAdHocCommandSession> command_;
			QtFormWidget* formWidget_;
			QBoxLayout* formLayout_;
			Form::ref form_;
			QLabel* label_;
			QPushButton* backButton_;
			QPushButton* nextButton_;
			QPushButton* completeButton_;
			QPushButton* cancelButton_;
			std::map<Command::Action, QPushButton*> actions_;
	};
}
