/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>

#include <Swift/Controllers/UIInterfaces/ContactEditWindow.h>
#include <Swiften/Base/String.h>
#include <Swiften/JID/JID.h>

class QLabel;
class QVBoxLayout;

namespace Swift {
	class QtContactEditWidget;

	class QtContactEditWindow : public QWidget, public ContactEditWindow {
			Q_OBJECT

		public:
			QtContactEditWindow();

			virtual void setContact(const JID& jid, const String& name, const std::vector<String>& groups, const std::set<String>& allGroups);

			void setEnabled(bool);
			void show();
			void hide();

		private slots:
			void handleRemoveContact();
			void handleUpdateContact();

		private:
			JID jid_;
			QVBoxLayout* groupsLayout_;
			QLabel* jidLabel_;
			QtContactEditWidget* contactEditWidget_;
	};
}

