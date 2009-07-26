#include "RosterItem.h"

#include <qdebug.h>

namespace Swift {
	
RosterItem::RosterItem(RosterItem* parent) : QObject() {
	parent_ = parent;
}

RosterItem::~RosterItem() {
	qDeleteAll(children_);
}

RosterItem* RosterItem::getParentItem() {
	return parent_;
}

void RosterItem::addChild(RosterItem* child) {
	printf("Boing\n");
	children_.append(child);
	connect(child, SIGNAL(changed()), this, SIGNAL(changed()));
	emit changed();
}

int RosterItem::rowCount() {
	qDebug() << "Returning size of " << children_.size() << " for item " << name_;
	return children_.size();
}

int RosterItem::rowOf(RosterItem* item) {
	return children_.indexOf(item);;
}

int RosterItem::row() {
	return parent_ ? parent_->rowOf(this) : 0;
}

RosterItem* RosterItem::getItem(int row) {
	qDebug() << "Returning row " << row << " from item " << name_;
	return children_[row];
}

QVariant RosterItem::data(int role) {
	if (role != Qt::DisplayRole) {
		return QVariant();
	}
	qDebug() << "Returning name " << name_ << " for role " << role;
	return name_;
}

void RosterItem::setName(QString name) {
	name_ = name;
	qDebug() << "Name changed to " << name;
	changed();
}
}