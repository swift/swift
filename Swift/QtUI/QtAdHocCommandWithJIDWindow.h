/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QDialog>
#include <QLineEdit>

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

class QBoxLayout;

namespace Swift {
	class UIEventStream;
	class QtFormWidget;
	class QtAdHocCommandWithJIDWindow : public QDialog {
		Q_OBJECT
		public:
			QtAdHocCommandWithJIDWindow(UIEventStream* eventStream);
			virtual ~QtAdHocCommandWithJIDWindow();
		public slots:
			void handleAcceptClick();
			void handleRejectClick();
		private:
			UIEventStream* uiEventStream_;
			QLineEdit* jid_;
			QLineEdit* node_;
	};
}
