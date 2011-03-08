/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "RosterModel.h"

#include <boost/bind.hpp>

#include <QColor>
#include <QIcon>
#include <qdebug.h>

#include "Swiften/Elements/StatusShow.h"
#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include <Swift/Controllers/StatusUtil.h>

#include "QtSwiftUtil.h"
#include "Swift/QtUI/Roster/QtTreeWidget.h"

namespace Swift {

RosterModel::RosterModel(QtTreeWidget* view) : view_(view) {
	roster_ = NULL;
}

RosterModel::~RosterModel() {
}

void RosterModel::setRoster(Roster* roster) {
	roster_ = roster;
	if (roster_) {
		roster->onChildrenChanged.connect(boost::bind(&RosterModel::handleChildrenChanged, this, _1));
		roster->onDataChanged.connect(boost::bind(&RosterModel::handleDataChanged, this, _1));
	}
	reLayout();
}

void RosterModel::reLayout() {
	//emit layoutChanged();
	reset();
	if (!roster_) {
		return;
	}
	foreach (RosterItem* item, roster_->getRoot()->getDisplayedChildren()) {
		GroupRosterItem* child = dynamic_cast<GroupRosterItem*>(item);
		if (!child) continue;
		emit itemExpanded(index(child), child->isExpanded());
	}
}

void RosterModel::handleChildrenChanged(GroupRosterItem* /*group*/) {
	reLayout();
}							  

void RosterModel::handleDataChanged(RosterItem* item) {
	Q_ASSERT(item);
	QModelIndex modelIndex = index(item);
	if (modelIndex.isValid()) {
		emit dataChanged(modelIndex, modelIndex);
	}
}

int RosterModel::columnCount(const QModelIndex& /*parent*/) const {
	return 1;
}

RosterItem* RosterModel::getItem(const QModelIndex& index) const {
	return index.isValid() ? static_cast<RosterItem*>(index.internalPointer()) : NULL;
}

QVariant RosterModel::data(const QModelIndex& index, int role) const {
	RosterItem* item = getItem(index);
	if (!item) return QVariant();

	switch (role) {
		case Qt::DisplayRole: return P2QSTRING(item->getDisplayName());
		case Qt::TextColorRole: return getTextColor(item);
		case Qt::BackgroundColorRole: return getBackgroundColor(item);
		case Qt::ToolTipRole: return getToolTip(item);
		case StatusTextRole: return getStatusText(item);
		case AvatarRole: return getAvatar(item);
		case PresenceIconRole: return getPresenceIcon(item);
		case ChildCountRole: return getChildCount(item);
	 	default: return QVariant();
	}
}

int RosterModel::getChildCount(RosterItem* item) const {
	GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
	return group ? group->getDisplayedChildren().size() : 0; 
}

QColor RosterModel::intToColor(int color) const {
	return QColor(
		((color & 0xFF0000)>>16),
		((color & 0xFF00)>>8), 
		(color & 0xFF));
}

QColor RosterModel::getTextColor(RosterItem* item) const {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	int color = 0;
	if (contact) {
		switch (contact->getStatusShow()) {
			case StatusShow::Online: color = 0x000000; break;
			case StatusShow::Away: color = 0x336699; break;
			case StatusShow::XA: color = 0x336699; break;
			case StatusShow::FFC: color = 0x000000; break;
			case StatusShow::DND: color = 0x990000; break;
			case StatusShow::None: color = 0x7F7F7F;break;
		}
	}
	return intToColor(color);
}

QColor RosterModel::getBackgroundColor(RosterItem* item) const {
	return dynamic_cast<ContactRosterItem*>(item) ? intToColor(0xFFFFFF) : intToColor(0x969696);
}

QString RosterModel::getToolTip(RosterItem* item) const {
	QString tip(P2QSTRING(item->getDisplayName()));
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (contact) {
		if (contact->getDisplayJID().isValid()) {
			tip += "\n" + P2QSTRING(contact->getDisplayJID().toBare().toString());
		}
		tip += "\n " + P2QSTRING(statusShowTypeToFriendlyName(contact->getStatusShow()));
		if (!getStatusText(item).isEmpty()) {
			tip += ": " + getStatusText(item);
		}
	}
	return tip;
}

QString RosterModel::getAvatar(RosterItem* item) const {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (!contact) {
		return "";
	}
	return QString(contact->getAvatarPath().c_str());
}

QString RosterModel::getStatusText(RosterItem* item) const {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (!contact) return "";
	return P2QSTRING(contact->getStatusText());
}

QIcon RosterModel::getPresenceIcon(RosterItem* item) const {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (!contact) return QIcon();
	QString iconString;
	switch (contact->getStatusShow()) {
	 	case StatusShow::Online: iconString = "online";break;
	 	case StatusShow::Away: iconString = "away";break;
	 	case StatusShow::XA: iconString = "away";break;
	 	case StatusShow::FFC: iconString = "online";break;
	 	case StatusShow::DND: iconString = "dnd";break;
	 	case StatusShow::None: iconString = "offline";break;
	}
	return QIcon(":/icons/" + iconString + ".png");
}


QModelIndex RosterModel::index(int row, int column, const QModelIndex& parent) const {
	if (!roster_) {
		return QModelIndex();
	}
	GroupRosterItem* parentItem;
	if (!parent.isValid()) {
		//top level
		parentItem = roster_->getRoot();
	} else {
		parentItem = dynamic_cast<GroupRosterItem*>(getItem(parent));
		if (!parentItem) return QModelIndex();
	}
	return static_cast<size_t>(row) < parentItem->getDisplayedChildren().size() ? createIndex(row, column, parentItem->getDisplayedChildren()[row]) : QModelIndex();
}

QModelIndex RosterModel::index(RosterItem* item) const {
	GroupRosterItem* parent = item->getParent();
	/* Recursive check that it's ok to create such an item 
		Assuming there are more contacts in a group than groups in a 
		group, this could save a decent chunk of search time at startup.*/
	if (parent == NULL || roster_ == NULL || (parent != roster_->getRoot() && !index(parent).isValid())) {
		return QModelIndex();
	}
	for (size_t i = 0; i < parent->getDisplayedChildren().size(); i++) {
		if (parent->getDisplayedChildren()[i] == item) {
			return createIndex(i, 0, item);
		}
	}
	return QModelIndex();
}

QModelIndex RosterModel::parent(const QModelIndex& child) const {
	if (!roster_ || !child.isValid()) {
		return QModelIndex();
	}
	
	GroupRosterItem* parent = getItem(child)->getParent();
	return (parent != roster_->getRoot()) ? index(parent) : QModelIndex();
}

int RosterModel::rowCount(const QModelIndex& parent) const {
	if (!roster_) return 0;
	RosterItem* item = parent.isValid() ? static_cast<RosterItem*>(parent.internalPointer()) : roster_->getRoot();
	Q_ASSERT(item);
	GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
	int count = group ? group->getDisplayedChildren().size() : 0;
//	qDebug() << "rowCount = " << count << " where parent.isValid() == " << parent.isValid() << ", group == " << (group ? P2QSTRING(group->getDisplayName()) : "*contact*");
	return count;
}

}
