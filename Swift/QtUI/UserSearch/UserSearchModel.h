/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QAbstractItemModel>
#include <QList>

#include "Swift/Controllers/Chat/UserSearchController.h"

namespace Swift {
	class UserSearchModel : public QAbstractItemModel {
		Q_OBJECT
		public:
			UserSearchModel();
			void clear();
			void setResults(const std::vector<UserSearchResult>& results);
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
			QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
			QModelIndex parent(const QModelIndex& index) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
		private:
			std::vector<UserSearchResult> results_;
	};

}
