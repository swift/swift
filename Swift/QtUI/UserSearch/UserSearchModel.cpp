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
	return data(result, role);
}

QVariant UserSearchModel::data(UserSearchResult* item, int role) {
	switch (role) {
		case Qt::DisplayRole: return QVariant(nameLine(item));
		case DetailTextRole: return QVariant(detailLine(item));
		default: return QVariant();
	}
}

QString UserSearchModel::nameLine(UserSearchResult* item) {
	QString result;
	const std::map<std::string, std::string> fields = item->getFields();
	std::map<std::string, std::string>::const_iterator first = fields.find("first");
	if (first != fields.end()) {
		result += P2QSTRING((*first).second);
	}
	std::map<std::string, std::string>::const_iterator last = fields.find("last");
	if (last != fields.end()) {
		if (!result.isEmpty()) {
			result += " ";
		}
		result += P2QSTRING((*last).second);
	}
	if (result.isEmpty()) {
		result = P2QSTRING(item->getJID().toString());
	}
	return result;
}

QString UserSearchModel::detailLine(UserSearchResult* item) {
	return P2QSTRING(item->getJID().toString());
}

QModelIndex UserSearchModel::index(int row, int column, const QModelIndex & parent) const {
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}
	return row < static_cast<int>(results_.size()) ? createIndex(row, column, reinterpret_cast<void*>(const_cast<UserSearchResult*>(&(results_[row])))) : QModelIndex();
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
