/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>

#include <Swift/Controllers/UIInterfaces/ContactEditWindow.h>
#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/VCard.h>

class QLabel;
class QVBoxLayout;

namespace Swift {
	class QtContactEditWidget;

	class QtContactEditWindow : public QWidget, public ContactEditWindow {
			Q_OBJECT

		public:
			QtContactEditWindow();
			virtual ~QtContactEditWindow();

			virtual void setNameSuggestions(const std::vector<std::string>& nameSuggestions);
			virtual void setContact(const JID& jid, const std::string& name, const std::vector<std::string>& groups, const std::set<std::string>& allGroups);

			void setEnabled(bool);
			void show();
			void hide();

			static bool confirmContactDeletion(const JID& jid);

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

