/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWizardPage>
#include <set>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchFieldsPage.h>

namespace Swift {
	class QtContactEditWidget;

	class QtUserSearchDetailsPage : public QWizardPage {
			Q_OBJECT
		public:
			QtUserSearchDetailsPage(const std::set<std::string>& availableGroups);

			std::set<std::string> getSelectedGroups();
			std::string getName();

			void clear();

		signals:
		  void onUserTriggersFinish();

		private:
		  QtContactEditWidget* editWidget;
	};
}
