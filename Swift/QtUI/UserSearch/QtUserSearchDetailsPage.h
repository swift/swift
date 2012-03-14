/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWizardPage>

#include <set>
#include <string>
#include <vector>

#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>
#include <Swift/QtUI/UserSearch/ui_QtUserSearchFieldsPage.h>

namespace Swift {
	class QtContactEditWidget;

	class QtUserSearchDetailsPage : public QWizardPage {
			Q_OBJECT
		public:
			QtUserSearchDetailsPage(const std::set<std::string>& availableGroups);
			virtual ~QtUserSearchDetailsPage();

			void setJID(const JID& jid);
			void setNameSuggestions(const std::vector<std::string>& nameSuggestions);

			std::set<std::string> getSelectedGroups();
			std::string getName();

			void clear();

		signals:
			void onUserTriggersFinish();

		private:
			QtContactEditWidget* editWidget;
			JID contactJID;
	};
}
