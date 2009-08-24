#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidget.h"

#include <qdebug.h>

namespace Swift {

QtTreeWidgetItem::QtTreeWidgetItem(QtTreeWidgetItem* parentItem) : QObject(), textColor_(0,0,0), backgroundColor_(255,255,255) {
	parent_ = parentItem;
	shown_ = true;
	expanded_ = true;
}

void QtTreeWidgetItem::setText(const String& text) {
	displayName_ = P2QSTRING(text);
}

void QtTreeWidgetItem::setStatusText(const String& text) {
	statusText_ = P2QSTRING(text);
}

void QtTreeWidgetItem::setAvatarPath(const String& path) {
	avatar_ = QIcon(P2QSTRING(path));
}

void QtTreeWidgetItem::setStatusShow(StatusShow::Type show) {
	statusShowType_ = show;
	int color = 0;
	switch (show) {
	 	case StatusShow::Online: color = 0x000000;break;
	 	case StatusShow::Away: color = 0x336699;break;
	 	case StatusShow::XA: color = 0x336699;break;
	 	case StatusShow::FFC: color = 0x000000;break;
	 	case StatusShow::DND: color = 0x990000;break;
	 	case StatusShow::None: color = 0x7F7F7F;break;
	}
	setTextColor(color);
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

void QtTreeWidgetItem::handleChanged(QtTreeWidgetItem* child) {
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
	 	case StatusTextRole: return statusText_;
		case AvatarRole: return avatar_;
		case PresenceIconRole: return getPresenceIcon();
	 	default: return QVariant();
	}
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

bool QtTreeWidgetItem::isContact() {
	return children_.size() == 0;
}

bool QtTreeWidgetItem::isExpanded() {
	return expanded_;
}

}
