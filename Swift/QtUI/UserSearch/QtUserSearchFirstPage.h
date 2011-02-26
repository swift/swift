/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWizardPage>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchFirstPage.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>

namespace Swift {
	class UserSearchModel;
	class UserSearchDelegate;
	class UserSearchResult;
	class UIEventStream;

	class QtUserSearchFirstPage : public QWizardPage, public Ui::QtUserSearchFirstPage {
		Q_OBJECT
		public:
			QtUserSearchFirstPage(UserSearchWindow::Type type, const QString& title);
			virtual bool isComplete() const;
		public slots:
			void emitCompletenessCheck();
	};
}
