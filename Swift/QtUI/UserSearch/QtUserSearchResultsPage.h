/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWizardPage>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchResultsPage.h>

namespace Swift {
	class QtUserSearchResultsPage : public QWizardPage, public Ui::QtUserSearchResultsPage {
		Q_OBJECT
		public:
			QtUserSearchResultsPage();
			virtual bool isComplete() const;
		signals:
		  void onUserTriggersContinue();
		public slots:
			void emitCompletenessCheck();
	};
}
