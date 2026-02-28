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
    using scoped_connection = boost::signals2::scoped_connection;
    connectionList_.emplace_back(std::make_unique<scoped_connection>(
        chattables_.onBeginAdd.connect([this](int index) {beginInsertRows(QModelIndex(), index, index);})
    ));
    connectionList_.emplace_back(std::make_unique<scoped_connection>(
        chattables_.onAdded.connect([this]() {endInsertRows();})
    ));
    connectionList_.emplace_back(std::make_unique<scoped_connection>(
        chattables_.onChanged.connect(
            [this](const JID& jid, int index) {
                auto modelIndex = createIndex(index, 0, const_cast<JID*>(&jid));
                dataChanged(modelIndex, modelIndex, {});
            }
        )
    ));
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
