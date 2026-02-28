/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <QBoxLayout>
#include <QPointer>
#include <QWidget>

#include <Swift/Controllers/Roster/FuzzyRosterFilter.h>
#include <Swift/Controllers/Roster/RosterFilter.h>

#include <Swift/QtUI/Roster/QtTreeWidget.h>

namespace Swift {
class UIEventStream;
class QtClosableLineEdit;
class QtFilterWidget : public QWidget {
    Q_OBJECT
    public:
        QtFilterWidget(QWidget* parent, QtTreeWidget* treeView, UIEventStream* eventStream, QBoxLayout* layout = nullptr);
        virtual ~QtFilterWidget();

    protected:
        bool eventFilter(QObject*, QEvent* event);

    private:
        void popAllFilters();
        void pushAllFilters();

        void updateRosterFilters();
        void updateSearchFilter();

        void handleFilterAdded(RosterFilter* filter);
        void handleFilterRemoved(RosterFilter* filter);

    private:
        QtClosableLineEdit* filterLineEdit_;
        QPointer<QtTreeWidget> treeView_;
        UIEventStream* eventStream_;
        std::vector<RosterFilter*> filters_;
        QAbstractItemModel* sourceModel_;
        FuzzyRosterFilter* fuzzyRosterFilter_;
        bool isModifierSinglePressed_;
};

}
