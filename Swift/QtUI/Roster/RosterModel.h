#pragma once

#include "Swift/QtUI/Roster/RosterItem.h"

#include <QAbstractItemModel>
#include <QList>

namespace Swift {
class RosterModel : public QAbstractItemModel {
public:
	RosterModel(RosterItem* tree);
	~RosterModel();
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
private:
	RosterItem* tree_;
};

}