/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2.hpp>

#include <QAbstractItemModel>

#include <Swiften/Elements/StatusShow.h>
#include <Swiften/JID/JID.h>

namespace Swift {

    class SwiftAccountData {
    public:
        struct SwiftAccountDataItem {
            SwiftAccountDataItem(std::string serverID) : serverID_(serverID) {}
            //FIXME eliminate serverID_, the userJID_ will be the ID when we connect with the actual data.
            std::string serverID_;
            QString iconPath_;
            JID userJID_;
            size_t unreadCount_ = 0;
            StatusShow status_ = StatusShow::None;
            boost::signals2::scoped_connection dataChangedConnection_;
            boost::signals2::signal<void ()> onDataChanged;
            void handleChangeStatusRequest(StatusShow::Type show, const std::string& /*statusText*/) {
                status_ = show;
                onDataChanged();
            }
        };
    public:
        SwiftAccountData() {}
        ~SwiftAccountData() {
            for (auto account : accounts_) {
                delete account;
            }
        }
        //FIXME make addAccount with SwiftAccountDataItem, and added after a succesfull connection to the server has been established.
        void addAccount(JID userJID) {
            SwiftAccountDataItem* newItem = new SwiftAccountDataItem(userJID);
            newItem->dataChangedConnection_ = newItem->onDataChanged.connect(boost::bind(&SwiftAccountData::handleDataChanged, this));
            accounts_.push_back(newItem);
        }
        SwiftAccountDataItem* getAccount(int index) {
            if (index >= accounts_.size()) {
                return nullptr;
            }
            return accounts_[index];
        }
        size_t size() {
            return accounts_.size();
        }
    public:
        boost::signals2::signal<void()> onDataChanged;
    private:
        void handleDataChanged() {
            onDataChanged();
        }
    private:
        QList<SwiftAccountDataItem*> accounts_;
    };

    class ServerListModel : public QAbstractItemModel {
        Q_OBJECT
        public:
            ServerListModel();
            ~ServerListModel() override;

            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
            QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
            QModelIndex parent(const QModelIndex& index) const override;

            QMimeData* mimeData(const QModelIndexList& indexes) const override;

            virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;

            void setModelData(SwiftAccountData* data) { modelData_ = data; }
            void handleDataChanged();
        private:
            SwiftAccountData* modelData_;
    };
}
