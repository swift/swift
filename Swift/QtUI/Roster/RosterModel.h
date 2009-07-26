#pragma once

#include "Swift/QtUI/Roster/RosterItem.h"

#include <QAbstractItemModel>
#include <QList>

namespace Swift {
class RosterModel : public QAbstractItemModel {
Q_OBJECT
public:
	RosterModel();
	~RosterModel();
	void setRoot(RosterItem* tree);
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
private slots:
	void handleItemChanged();
private:
	RosterItem* tree_;
};

}