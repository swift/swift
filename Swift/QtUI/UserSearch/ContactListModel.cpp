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

#include <Swift/QtUI/UserSearch/ContactListModel.h>

#include <QMimeData>

#include <Swiften/Base/Path.h>
#include <Swiften/Elements/StatusShow.h>

#include <Swift/QtUI/QtResourceHelper.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QDataStream& operator >>(QDataStream& in, StatusShow::Type& e){
    quint32 buffer;
    in >> buffer;
    switch(buffer) {
        case StatusShow::Online:
            e = StatusShow::Online;
            break;
        case StatusShow::Away:
            e = StatusShow::Away;
            break;
        case StatusShow::FFC:
            e = StatusShow::FFC;
            break;
        case StatusShow::XA:
            e = StatusShow::XA;
            break;
        case StatusShow::DND:
            e = StatusShow::DND;
            break;
        default:
            e = StatusShow::None;
            break;
    }
    return in;
}

ContactListModel::ContactListModel(bool editable) : QAbstractItemModel(), editable_(editable) {
}

void ContactListModel::setList(const std::vector<Contact::ref>& list) {
    emit layoutAboutToBeChanged();
    contacts_ = list;
    emit layoutChanged();
}

const std::vector<Contact::ref>& ContactListModel::getList() const {
    return contacts_;
}

Contact::ref ContactListModel::getContact(const size_t i) const {
    return contacts_[i];
}

Qt::ItemFlags ContactListModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.isValid()) {
        flags = flags & ~Qt::ItemIsDropEnabled;
    } else {
        flags = Qt::ItemIsDropEnabled | flags;
    }
    return flags;
}

int ContactListModel::columnCount(const QModelIndex&) const {
    return editable_ ? 2 : 1;
}

QVariant ContactListModel::data(const QModelIndex& index, int role) const {
    if ((boost::numeric_cast<size_t>(index.row()) < contacts_.size()) && (index.column() == 0)) {
        const Contact::ref& contact = contacts_[index.row()];
        if (role == Qt::EditRole) {
            return P2QSTRING(contact->jid.toString());
        }
        return dataForContact(contact, role);
    } else {
        return QVariant();
    }
}

QModelIndex ContactListModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    return boost::numeric_cast<size_t>(row) < contacts_.size() ? createIndex(row, column, contacts_[row].get()) : QModelIndex();
}

QModelIndex ContactListModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) {
        return QModelIndex();
    }
    return QModelIndex();
}

int ContactListModel::rowCount(const QModelIndex& /*parent*/) const {
    return contacts_.size();
}

bool ContactListModel::removeRows(int row, int /*count*/, const QModelIndex& /*parent*/) {
    if (boost::numeric_cast<size_t>(row) < contacts_.size()) {
        emit layoutAboutToBeChanged();
        contacts_.erase(contacts_.begin() + row);
        emit layoutChanged();
        onListChanged(getList());
        return true;
    }
    return false;
}

QVariant ContactListModel::dataForContact(const Contact::ref& contact, int role) const {
    switch (role) {
        case Qt::DisplayRole: return P2QSTRING(contact->name);
        case DetailTextRole: return P2QSTRING(contact->jid.toString());
        case AvatarRole: return QVariant(P2QSTRING(pathToString(contact->avatarPath)));
        case PresenceIconRole: return getPresenceIconForContact(contact);
        default: return QVariant();
    }
}

QIcon ContactListModel::getPresenceIconForContact(const Contact::ref& contact) const {
    return QIcon(statusShowTypeToIconPath(contact->statusType));
}

}
