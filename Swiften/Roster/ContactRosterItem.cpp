/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Roster/GroupRosterItem.h"

namespace Swift {


ContactRosterItem::ContactRosterItem(const JID& jid, const String& name, GroupRosterItem* parent, TreeWidgetFactory* factory) : jid_(jid), name_(name) {
	parent->addChild(this);
	widget_ = factory->createTreeWidgetItem(parent->getWidget());
	widget_->setText(name.isEmpty() ? jid.toString() : name);
	widget_->onUserAction.connect(boost::bind(&ContactRosterItem::handleUserAction, this, _1));
	setStatusShow(StatusShow::None);
}

ContactRosterItem::~ContactRosterItem() {
	delete widget_;
}

void ContactRosterItem::setName(const String& name) {
	widget_->setText(name);
}

StatusShow::Type ContactRosterItem::getStatusShow() {
	return statusShow_;
}

void ContactRosterItem::setStatusShow(StatusShow::Type show) {
	statusShow_ = show;
	widget_->setStatusShow(show);
}

void ContactRosterItem::setStatusText(const String& status) {
	widget_->setStatusText(status);
}

void ContactRosterItem::setAvatarPath(const String& path) {
	widget_->setAvatarPath(path);
}

const JID& ContactRosterItem::getJID() const {
	return jid_;
}

void ContactRosterItem::show() {
	widget_->show();
}

void ContactRosterItem::hide() {
	widget_->hide();
}

}


