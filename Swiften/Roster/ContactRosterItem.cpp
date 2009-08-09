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

StatusShow::Type ContactRosterItem::getStatusShow() {
	return statusShow_;
}

void ContactRosterItem::setStatusShow(StatusShow::Type show) {
	statusShow_ = show;
	int colour = 0;
	switch (show) {
		case StatusShow::Online: colour = 0x000000;break;
		case StatusShow::Away: colour = 0x336699;break;
		case StatusShow::XA: colour = 0x336699;break;
		case StatusShow::FFC: colour = 0x000000;break;
		case StatusShow::DND: colour = 0x990000;break;
		case StatusShow::None: colour = 0x7F7F7F;break;
	}
	widget_->setTextColor(colour);
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


