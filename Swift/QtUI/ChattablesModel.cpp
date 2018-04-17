/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ChattablesModel.h>

#include <QDebug>

#include <Swift/Controllers/Chat/Chattables.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

ChattablesModel::ChattablesModel(Chattables& chattables, QObject* parent) : QAbstractListModel(parent), chattables_(chattables) {
    //FIXME: scoped connections, do it properly not reset.
    chattables_.onAdded.connect([this](const JID& /*jid*/) {beginResetModel(); endResetModel();});
    chattables_.onRemoved.connect([this](const JID& /*jid*/) {beginResetModel(); endResetModel();});
    chattables_.onChanged.connect([this](const JID& /*jid*/) {beginResetModel(); endResetModel();});
}

int ChattablesModel::rowCount(const QModelIndex& /*parent*/) const {
    return chattables_.get().size();
}

QVariant ChattablesModel::data(const QModelIndex& index, int role) const {
    //FIXME: Check validity
    auto state = chattables_.getState(chattables_.get()[index.row()]);
    if (role == Qt::DisplayRole) {
        return P2QSTRING((state.name.empty() ? state.jid.toString() : state.name));
    }
    if (role == UnreadCountRole) {
        return QString::number(state.unreadCount);
    }
    if (role == TypeRole) {
        switch (state.type) {
            case Chattables::State::Type::Room: return "ROOM";
            case Chattables::State::Type::Person: return "PERSON";
        }
    }
    if (role == StatusRole) {
        return QVariant(static_cast<int>(state.status));
    }
    if (role == JIDRole) {
        return P2QSTRING(state.jid.toString());
    }
    return QVariant();
}

}
