/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include <QAbstractItemModel>

#include <Swift/Controllers/Contact.h>

#include <Swift/QtUI/ChatList/ChatListGroupItem.h>
#include <Swift/QtUI/ChatList/ChatListItem.h>
#include <Swift/QtUI/ChatList/ChatListRecentItem.h>

namespace Swift {
    class ContactListModel : public QAbstractItemModel {
        Q_OBJECT
        public:
            enum ContactRoles {
                DetailTextRole = Qt::UserRole,
                AvatarRole = Qt::UserRole + 1,
                PresenceIconRole = Qt::UserRole + 2
            };

        public:
            ContactListModel(bool editable);

            void setList(const std::vector<Contact::ref>& list);
            const std::vector<Contact::ref>& getList() const;
            Contact::ref getContact(const size_t i) const;

            Qt::ItemFlags flags(const QModelIndex& index) const;
            int columnCount(const QModelIndex& parent = QModelIndex()) const;
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
            QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            QModelIndex parent(const QModelIndex& index) const;
            int rowCount(const QModelIndex& parent = QModelIndex()) const;
            bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

        private:
            QVariant dataForContact(const Contact::ref& contact, int role) const;
            QIcon getPresenceIconForContact(const Contact::ref& contact) const;

        signals:
            void onListChanged(std::vector<Contact::ref> list);
            void onJIDsDropped(const std::vector<JID>& contact);

        private:
            bool editable_;
            std::vector<Contact::ref> contacts_;
    };

}
