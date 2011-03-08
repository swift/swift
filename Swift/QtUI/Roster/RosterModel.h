/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Roster/Roster.h"

#include <QAbstractItemModel>
#include <QList>

namespace Swift {
	enum RosterRoles {
		StatusTextRole = Qt::UserRole,
		AvatarRole = Qt::UserRole + 1,
		PresenceIconRole = Qt::UserRole + 2,
		StatusShowTypeRole = Qt::UserRole + 3,
		ChildCountRole = Qt::UserRole + 4,
	};

	class QtTreeWidget;

	class RosterModel : public QAbstractItemModel {
		Q_OBJECT
		public:
			RosterModel(QtTreeWidget* view);
			~RosterModel();
			void setRoster(Roster* swiftRoster);
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
			QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
			QModelIndex index(RosterItem* item) const;
			QModelIndex parent(const QModelIndex& index) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
		signals:
			void itemExpanded(const QModelIndex& item, bool expanded);
		private:
			void handleDataChanged(RosterItem* item);
			void handleChildrenChanged(GroupRosterItem* item);
			RosterItem* getItem(const QModelIndex& index) const;
			QColor intToColor(int color) const;
			QColor getTextColor(RosterItem* item) const;
			QColor getBackgroundColor(RosterItem* item) const;
			QString getToolTip(RosterItem* item) const;
			QString getAvatar(RosterItem* item) const;
			QString getStatusText(RosterItem* item) const;
			QIcon getPresenceIcon(RosterItem* item) const;
			int getChildCount(RosterItem* item) const;
			void reLayout();
			Roster* roster_;
			QtTreeWidget* view_;
	};
}
