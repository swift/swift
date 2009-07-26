#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidget.h"

#include <qdebug.h>

namespace Swift {

QtTreeWidgetItem::QtTreeWidgetItem(QtTreeWidgetItem* parentItem) : QObject() {
	parent_ = parentItem;
}

void QtTreeWidgetItem::setText(const String& text) {
	displayName_ = P2QSTRING(text);
	statusText_ = displayName_ + " went away";
}

void QtTreeWidgetItem::setStatusText(const String& text) {
	statusText_ = P2QSTRING(text);
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

void QtTreeWidgetItem::setExpanded(bool b) {
	//treeWidget()->setItemExpanded(this, b);
}

void QtTreeWidgetItem::hide() {
	//setHidden(true);
}

void QtTreeWidgetItem::show() {
	//setHidden(false);
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
	qDeleteAll(children_);
}

QtTreeWidgetItem* QtTreeWidgetItem::getParentItem() {
	return parent_;
}

void QtTreeWidgetItem::addChild(QtTreeWidgetItem* child) {
	printf("Boing\n");
	children_.append(child);
	connect(child, SIGNAL(changed()), this, SIGNAL(changed()));
	emit changed();
}

int QtTreeWidgetItem::rowCount() {
	qDebug() << "Returning size of " << children_.size() << " for item " << displayName_;
	return children_.size();
}

int QtTreeWidgetItem::rowOf(QtTreeWidgetItem* item) {
	return children_.indexOf(item);
}

int QtTreeWidgetItem::row() {
	return parent_ ? parent_->rowOf(this) : 0;
}

QtTreeWidgetItem* QtTreeWidgetItem::getItem(int row) {
	qDebug() << "Returning row " << row << " from item " << displayName_;
	return children_[row];
}

QVariant QtTreeWidgetItem::data(int role) {
 	switch (role) {
	 	case Qt::DisplayRole: return displayName_;
	 	case StatusTextRole: return statusText_;
	 	default: return QVariant();
	}
}

bool QtTreeWidgetItem::isContact() {
	return children_.size() == 0;
}

}