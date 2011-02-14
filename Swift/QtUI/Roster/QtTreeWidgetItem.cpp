/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidget.h"

#include <qdebug.h>
#include <QtAlgorithms>
#include <algorithm>

namespace Swift {

QtTreeWidgetItem::QtTreeWidgetItem(QtTreeWidgetItem* parentItem) : QObject(), textColor_(0,0,0), backgroundColor_(255,255,255) {
	parent_ = parentItem;
	shown_ = true;
	expanded_ = true;
}


void QtTreeWidgetItem::setText(const std::string& text) {
	displayName_ = P2QSTRING(text);
	displayNameLower_ = displayName_.toLower();
	emit changed(this);
}

void QtTreeWidgetItem::setStatusText(const std::string& text) {
	statusText_ = P2QSTRING(text);
	emit changed(this);
}

void QtTreeWidgetItem::setAvatarPath(const std::string& path) {
	avatar_ = QIcon(P2QSTRING(path));
	emit changed(this);
}

void QtTreeWidgetItem::setStatusShow(StatusShow::Type show) {
	statusShowType_ = show;
	int color = 0;
	switch (show) {
		case StatusShow::Online: color = 0x000000; mergedShowType_ = StatusShow::Online; break;
		case StatusShow::Away: color = 0x336699; mergedShowType_ = StatusShow::Away; break;
	 	case StatusShow::XA: color = 0x336699; mergedShowType_ = StatusShow::Away; break;
		case StatusShow::FFC: color = 0x000000; mergedShowType_ = StatusShow::Online; break;
		case StatusShow::DND: color = 0x990000; mergedShowType_ = show; break;
		case StatusShow::None: color = 0x7F7F7F; mergedShowType_ = show; break;
	}
	setTextColor(color);
	emit changed(this);
}

void QtTreeWidgetItem::setTextColor(unsigned long color) {
	textColor_ = QColor(
	 					((color & 0xFF0000)>>16),
	 					((color & 0xFF00)>>8), 
	 					(color & 0xFF));
}

void QtTreeWidgetItem::setBackgroundColor(unsigned long color) {
	backgroundColor_ = QColor(
	 					((color & 0xFF0000)>>16),
	 					((color & 0xFF00)>>8), 
	 					(color & 0xFF));
}

void QtTreeWidgetItem::setExpanded(bool expanded) {
	expanded_ = expanded;
	emit changed(this);
}

void QtTreeWidgetItem::hide() {
	shown_ = false;
	emit changed(this);
}

void QtTreeWidgetItem::show() {
	shown_ = true;
	emit changed(this);
}

bool QtTreeWidgetItem::isShown() {
	return isContact() ? shown_ : shownChildren_.size() > 0;
}

QWidget* QtTreeWidgetItem::getCollapsedRosterWidget() {
	QWidget* widget = new QWidget();
	return widget;
}

QWidget* QtTreeWidgetItem::getExpandedRosterWidget() {
	QWidget* widget = new QWidget();
	return widget;
}

QtTreeWidgetItem::~QtTreeWidgetItem() {
	//It's possible (due to the way the roster deletes items in unknown order when it is deleted)
	// That the children will be deleted before the groups, or that the groups are deleted 
	// before the children. If the children are deleted first, they will let the parent know that
	// They've been deleted. If the parent is deleted first, it must tell the children not to
	// tell it when they're deleted. Everything will be deleted in the end, because all the
	// widgets are owned by the Roster in Swiften.
	if (parent_) {
		parent_->removeChild(this);
	}

	for (int i = 0; i < children_.size(); i++) {
		children_[i]->parentItemHasBeenDeleted();
	}
}

void QtTreeWidgetItem::parentItemHasBeenDeleted() {
	parent_ = NULL;
}

QtTreeWidgetItem* QtTreeWidgetItem::getParentItem() {
	return parent_;
}

void QtTreeWidgetItem::addChild(QtTreeWidgetItem* child) {
	children_.append(child);
	connect(child, SIGNAL(changed(QtTreeWidgetItem*)), this, SLOT(handleChanged(QtTreeWidgetItem*)));
	handleChanged(child);
}

void QtTreeWidgetItem::removeChild(QtTreeWidgetItem* child) {
	children_.removeAll(child);
	handleChanged(this);
}

void bubbleSort(QList<QtTreeWidgetItem*>& list) {
	bool done = false;
	for (int i = 0; i < list.size() - 1 && !done; i++) {
		done = true;
		for (int j = i + 1; j < list.size(); j++) {
			if (*(list[j]) < *(list[j - 1])) {
				done = false;
				QtTreeWidgetItem* lower = list[j];
				list[j] = list[j - 1];
				list[j - 1] = lower;
			}
		}
	}
}

void QtTreeWidgetItem::handleChanged(QtTreeWidgetItem* child) {
	/*We don't use the much faster qStableSort because it causes changed(child) and all sorts of nasty recursion*/
	//qStableSort(children_.begin(), children_.end(), itemLessThan);
	//bubbleSort(children_);
	std::stable_sort(children_.begin(), children_.end(), itemLessThan);
	shownChildren_.clear();
	for (int i = 0; i < children_.size(); i++) {
		if (children_[i]->isShown()) {
			shownChildren_.append(children_[i]);
		}
	}
	emit changed(child);
}

int QtTreeWidgetItem::rowCount() {
	//qDebug() << "Returning size of " << children_.size() << " for item " << displayName_;
	return shownChildren_.size();
}

int QtTreeWidgetItem::rowOf(QtTreeWidgetItem* item) {
	return shownChildren_.indexOf(item);
}

int QtTreeWidgetItem::row() {
	return parent_ ? parent_->rowOf(this) : 0;
}

QtTreeWidgetItem* QtTreeWidgetItem::getItem(int row) {
	//qDebug() << "Returning row " << row << " from item " << displayName_;
	Q_ASSERT(row >= 0);
	Q_ASSERT(row < rowCount());
	return shownChildren_[row];
}


QVariant QtTreeWidgetItem::data(int role) {
	if (!isContact()) {
		setTextColor(0xFFFFFF);
		setBackgroundColor(0x969696);
	}
 	switch (role) {
	 	case Qt::DisplayRole: return displayName_;
		case Qt::TextColorRole: return textColor_;
		case Qt::BackgroundColorRole: return backgroundColor_;
		case Qt::ToolTipRole: return isContact() ? toolTipString() : QVariant();
	 	case StatusTextRole: return statusText_;
		case AvatarRole: return avatar_;
		case PresenceIconRole: return getPresenceIcon();
	 	default: return QVariant();
	}
}

QString QtTreeWidgetItem::toolTipString() {
	return displayName_ + "\n " + statusText_;
}

QIcon QtTreeWidgetItem::getPresenceIcon() {
	QString iconString;
	switch (statusShowType_) {
	 	case StatusShow::Online: iconString = "online";break;
	 	case StatusShow::Away: iconString = "away";break;
	 	case StatusShow::XA: iconString = "away";break;
	 	case StatusShow::FFC: iconString = "online";break;
	 	case StatusShow::DND: iconString = "dnd";break;
	 	case StatusShow::None: iconString = "offline";break;
	}
	return QIcon(":/icons/" + iconString + ".png");
}

bool QtTreeWidgetItem::isContact() const {
	return children_.size() == 0;
}

bool QtTreeWidgetItem::isExpanded() {
	return expanded_;
}

bool QtTreeWidgetItem::operator<(const QtTreeWidgetItem& item) const {
	if (isContact()) {
		if (!item.isContact()) {
			return false;
		}
		return getStatusShowMerged() == item.getStatusShowMerged() ? getLowerName() < item.getLowerName() : getStatusShowMerged() < item.getStatusShowMerged();
	} else {
		if (item.isContact()) {
			return true;
		}
		return getLowerName() < item.getLowerName();
	}
}

bool itemLessThan(QtTreeWidgetItem* left, QtTreeWidgetItem* right) {
	return *left < *right;
}

}
