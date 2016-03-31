/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QAbstractItemModel>
#include <QList>

#include <Swift/Controllers/Roster/Roster.h>

#include <Swift/QtUI/QtScaledAvatarCache.h>

namespace Swift {
    enum RosterRoles {
        StatusTextRole = Qt::UserRole,
        AvatarRole = Qt::UserRole + 1,
        PresenceIconRole = Qt::UserRole + 2,
        StatusShowTypeRole = Qt::UserRole + 3,
        ChildCountRole = Qt::UserRole + 4,
        IdleRole = Qt::UserRole + 5,
        JIDRole = Qt::UserRole + 6,
        DisplayJIDRole = Qt::UserRole + 7
    };

    class QtTreeWidget;

    class RosterModel : public QAbstractItemModel {
        Q_OBJECT
        public:
            RosterModel(QtTreeWidget* view, bool screenReaderMode);
            ~RosterModel();
            void setRoster(Roster* swiftRoster);
            Qt::ItemFlags flags(const QModelIndex& index) const;
            int columnCount(const QModelIndex& parent = QModelIndex()) const;
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
            QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            QModelIndex index(RosterItem* item) const;
            QModelIndex parent(const QModelIndex& index) const;
            int rowCount(const QModelIndex& parent = QModelIndex()) const;
            QMimeData* mimeData(const QModelIndexList& indexes) const;

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
            QString getJID(RosterItem* item) const;
            QString getDisplayJID(RosterItem* item) const;
            QIcon getPresenceIcon(RosterItem* item) const;
            int getChildCount(RosterItem* item) const;
            bool getIsIdle(RosterItem* item) const;
            void reLayout();
            /** calculates screenreader-friendly text if in screenreader mode, otherwise uses alternative text */
            QString getScreenReaderTextOr(RosterItem* item, const QString& alternative) const;
        private:
            Roster* roster_;
            QtTreeWidget* view_;
            QtScaledAvatarCache* cachedImageScaler_;
            bool screenReader_;
    };
}
