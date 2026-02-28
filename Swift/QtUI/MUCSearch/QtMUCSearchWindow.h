/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/MUCSearchWindow.h>

#include <Swift/QtUI/MUCSearch/ui_QtMUCSearchWindow.h>

class QSortFilterProxyModel;

namespace Swift {
    class MUCSearchModel;
    class MUCSearchDelegate;
    class MUCSearchRoomItem;

    class QtMUCSearchWindow : public QDialog, public MUCSearchWindow {
        Q_OBJECT
        public:
            QtMUCSearchWindow();
            virtual ~QtMUCSearchWindow();

            virtual void clearList();
            virtual void addService(const MUCService& service);
            virtual void addSavedServices(const std::list<JID>& services);
            virtual void setSearchInProgress(bool searching);

            virtual void show();
            virtual void accept();
            virtual void reject();

        protected:
            virtual void resizeEvent(QResizeEvent* event);

        private slots:
            void handleSearch();
            void handleSearch(const QString&);
            void handleFilterStringChanged(const QString&);
            void handleActivated(const QModelIndex& index);
            void updateThrobberPosition();
            void handleSelectionChanged (const QItemSelection&, const QItemSelection&);
            MUCSearchRoomItem* getSelectedRoom() const;

        private:
            Ui::QtMUCSearchWindow ui_;
            MUCSearchModel* model_;
            MUCSearchDelegate* delegate_;
            QLabel* throbber_;
            bool hasHadScrollBars_;
            QSortFilterProxyModel* sortFilterProxyModel_ = nullptr;
    };
}
