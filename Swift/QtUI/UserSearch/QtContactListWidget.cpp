/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/UserSearch/QtContactListWidget.h>

#include <Swift/QtUI/UserSearch/ContactListModel.h>
#include <Swift/QtUI/UserSearch/ContactListDelegate.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/QtUI/QtVCardWidget/QtRemovableItemDelegate.h>

#include <QHeaderView>

namespace Swift {

QtContactListWidget::QtContactListWidget(QWidget* parent, SettingsProvider* settings) : QTreeView(parent), settings_(settings), limited_(false) {
	contactListModel_ = new ContactListModel(true);
	setModel(contactListModel_);

	connect(contactListModel_, SIGNAL(onListChanged(std::vector<Contact::ref>)), this, SIGNAL(onListChanged(std::vector<Contact::ref>)));
	connect(contactListModel_, SIGNAL(onJIDsDropped(std::vector<JID>)), this, SIGNAL(onJIDsAdded(std::vector<JID>)));

	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setUniformRowHeights(true);

	setAlternatingRowColors(true);
	setIndentation(0);
	setHeaderHidden(true);
	setExpandsOnDoubleClick(false);
	setItemsExpandable(false);
	setEditTriggers(QAbstractItemView::DoubleClicked);

	contactListDelegate_ = new ContactListDelegate(settings->getSetting(QtUISettingConstants::COMPACT_ROSTER));
	removableItemDelegate_ = new QtRemovableItemDelegate(style());

	setItemDelegateForColumn(0, contactListDelegate_);
	setItemDelegateForColumn(1, removableItemDelegate_);

	header()->resizeSection(1, removableItemDelegate_->sizeHint(QStyleOptionViewItem(), QModelIndex()).width());

	header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
	header()->setResizeMode(0, QHeaderView::Stretch);
#endif
}

QtContactListWidget::~QtContactListWidget() {
	delete contactListDelegate_;
	delete removableItemDelegate_;
}

void QtContactListWidget::setList(const std::vector<Contact::ref>& list) {
	contactListModel_->setList(list);
}

std::vector<Contact::ref> QtContactListWidget::getList() const {
	return contactListModel_->getList();
}

Contact::ref QtContactListWidget::getContact(const size_t i) {
	return contactListModel_->getContact(i);
}

void QtContactListWidget::setMaximumNoOfContactsToOne(bool limited) {
	limited_ = limited;
}

void QtContactListWidget::updateContacts(const std::vector<Contact::ref>& contactUpdates) {
	std::vector<Contact::ref> contacts = contactListModel_->getList();
	foreach(const Contact::ref& contactUpdate, contactUpdates) {
		for(size_t n = 0; n < contacts.size(); n++) {
			if (contactUpdate->jid == contacts[n]->jid) {
				contacts[n] = contactUpdate;
				break;
			}
		}
	}
	contactListModel_->setList(contacts);
}

void QtContactListWidget::handleSettingsChanged(const std::string&) {
	contactListDelegate_->setCompact(settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));
}

}
