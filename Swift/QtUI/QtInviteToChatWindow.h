/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/InviteToChatWindow.h>

#include <QDialog>
#include <QStringList>

class QLineEdit;
class QBoxLayout;

namespace Swift {
	class QtInviteToChatWindow : public QDialog, public InviteToChatWindow {
		Q_OBJECT
		public:
			QtInviteToChatWindow(QWidget* parent = NULL);
			virtual ~QtInviteToChatWindow();

			virtual std::string getReason() const;

			virtual std::vector<JID> getJIDs() const;
		private:
			void addJIDLine();
		private slots:
			void handleJIDTextChanged();
			void handleAccepting();
			void handleRejecting();
		private:
			QStringList completions_;
			QLineEdit* reason_;
			QBoxLayout* jidsLayout_;
			std::vector<QLineEdit*> jids_;
	};
}


