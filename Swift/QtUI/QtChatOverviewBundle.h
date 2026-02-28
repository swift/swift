/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>

#include <QSortFilterProxyModel>
#include <QString>
#include <QWidget>

#include <Swiften/JID/JID.h>

class QListView;

namespace Swift {
    class ChattablesModel;
    class QtClickableLabel;
    class QtExpandedListView;

    class BundleFilter : public QSortFilterProxyModel {
        Q_OBJECT
        public:
            enum class Filter {Unread, People, Rooms, Online};
            BundleFilter(QObject* parent);
            void addFilter(Filter);
            bool hasFilter(Filter);
            void removeFilter(Filter);
        protected:
            bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
        private:
            std::set<Filter> filters_;
    };

    class QtChatOverviewBundle : public QWidget {
            Q_OBJECT
        public:
            QtChatOverviewBundle(ChattablesModel*, QString name, bool hideWhenEmpty, QWidget* parent);
            ~QtChatOverviewBundle() override;

        signals:
            void clicked(JID jid);

        private slots:
            void handleFilterClicked();
            void handleItemClicked(const QModelIndex&);

        private:
            void updateVisibility();

        private:
            ChattablesModel* rootModel_;
            QtExpandedListView* listView_;
            BundleFilter* proxyModel_;
            bool hideWhenEmpty_;
            QtClickableLabel* filterLabel_ = nullptr;
    };
}
