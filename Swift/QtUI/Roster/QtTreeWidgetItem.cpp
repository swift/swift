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
	qDebug() << "Setting avatar to " << P2QSTRING(path);
	avatar_ = QIcon(P2QSTRING(path));
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
	parent_->removeChild(this);
	qDeleteAll(children_);
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
 	switch (role) {
	 	case Qt::DisplayRole: return displayName_;
		case Qt::TextColorRole: return textColor_;
		case Qt::BackgroundColorRole: return backgroundColor_;
	 	case StatusTextRole: return statusText_;
		case AvatarRole: return avatar_;
	 	default: return QVariant();
	}
}

bool QtTreeWidgetItem::isContact() {
	return children_.size() == 0;
}

bool QtTreeWidgetItem::isExpanded() {
	return expanded_;
}

}