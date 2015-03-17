/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWidget>
#include <QRect>
#include <QShortcut>

#include <Swift/QtUI/QtChatTabsBase.h>

class QTabWidget;
class QMenu;

namespace Swift {
	class SettingsProvider;

	class QtTabbable;
	class QtTabWidget;
	class QtDynamicGridLayout;
	class QtGridSelectionDialog;

	class QtChatTabs : public QWidget, public QtChatTabsBase {
		Q_OBJECT
		public:
			QtChatTabs(bool singleWindow, SettingsProvider* settingsProvider, bool trellisMode);
			virtual ~QtChatTabs();

			virtual void addTab(QtTabbable* tab);
			void minimise();
			QtTabbable* getCurrentTab();
			void setViewMenu(QMenu* viewMenu);

		signals:
			void geometryChanged();
			void onTitleChanged(const QString& title);

		protected slots:
			void closeEvent(QCloseEvent* event);
			void resizeEvent(QResizeEvent* event);
			void moveEvent(QMoveEvent* event);

		private slots:
			void handleCurrentTabIndexChanged(int newIndex);
			void handleTabClosing();
			void handleTabTitleUpdated();
			void handleTabTitleUpdated(QWidget* widget);
			void handleTabCloseRequested(int index);
			void handleWidgetShown();
			void handleWantsToActivate();
			void handleRequestedPreviousTab();
			void handleRequestedNextTab();
			void handleRequestedActiveTab();
			void flash();

			void handleOpenLayoutChangeDialog();

			void handleCloseTabShortcut();

		private:
			void storeTabPositions();
			void checkForFirstShow();

		private:
			bool singleWindow_;
			SettingsProvider* settingsProvider_;
			bool trellisMode_;
			QtDynamicGridLayout* dynamicGrid_;
			QtGridSelectionDialog* gridSelectionDialog_;

			QList<QShortcut*> shortcuts_;
	};
}

