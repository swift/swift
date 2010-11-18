/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/UserSearch/UserSearchModel.h"

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

UserSearchModel::UserSearchModel() {
}

void UserSearchModel::clear() {
	emit layoutAboutToBeChanged();
	results_.clear();
	emit layoutChanged();
}

void UserSearchModel::setResults(const std::vector<UserSearchResult>& results) {
	clear();
	emit layoutAboutToBeChanged();
	results_ = results;
	emit layoutChanged();
}

int UserSearchModel::columnCount(const QModelIndex& /*parent*/) const {
	return 1;
}

QVariant UserSearchModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid()) return QVariant();
	UserSearchResult* result = static_cast<UserSearchResult*>(index.internalPointer());
	switch (role) {
		case Qt::DisplayRole: return QVariant(P2QSTRING(result->getJID().toString()));
		default: return QVariant();
	}
}

QModelIndex UserSearchModel::index(int row, int column, const QModelIndex & parent) const {
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}
	return row < (int)results_.size() ? createIndex(row, column, (void*)&(results_[row])) : QModelIndex();
}

QModelIndex UserSearchModel::parent(const QModelIndex& /*index*/) const {
	return QModelIndex();
}

int UserSearchModel::rowCount(const QModelIndex& parentIndex) const {
	if (!parentIndex.isValid()) {
		return results_.size();
	}
	return 0;
}

}
