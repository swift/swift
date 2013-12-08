/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>

#include <QBoxLayout>
#include <QLineEdit>
#include <QWidget>

#include <Swift/Controllers/Roster/RosterFilter.h>
#include <Swift/Controllers/Roster/FuzzyRosterFilter.h>

#include <Swift/QtUI/Roster/QtTreeWidget.h>

namespace Swift {

class QtFilterWidget : public QWidget {
	Q_OBJECT
	public:
		QtFilterWidget(QWidget* parent, QtTreeWidget* treeView, QBoxLayout* layout = 0);
		virtual ~QtFilterWidget();

	protected:
		bool eventFilter(QObject*, QEvent* event);

	private:
		void popAllFilters();
		void pushAllFilters();

		void updateSearchFilter();

	private:
		QLineEdit* filterLineEdit_;
		QtTreeWidget* treeView_;
		std::vector<RosterFilter*> filters_;
		QAbstractItemModel* sourceModel_;
		FuzzyRosterFilter* fuzzyRosterFilter_;
		bool isModifierSinglePressed_;
};

}
