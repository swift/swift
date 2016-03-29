/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QList>
#include <QWidget>

#include <Swift/QtUI/QtChatTabsBase.h>

class QShortcut;

namespace Swift {

class QtChatTabsShortcutOnlySubstitute : public QWidget, public QtChatTabsBase {
	Q_OBJECT

	public:
		QtChatTabsShortcutOnlySubstitute();
		virtual ~QtChatTabsShortcutOnlySubstitute();

		virtual void addTab(QtTabbable* tab);

	private slots:
		void handleCloseTabShortcut();
		void handleRequestedNextTab();
		void handleRequestedActiveTab();
		void handleRequestedPreviousTab();

	private:
		QList<QtTabbable*> tabbableWindows() const;

	private:
		QList<QShortcut*> shortcuts_;
};

}
