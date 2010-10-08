/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtSetGroupsDialog.h"

#include <algorithm>

#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtSetGroupsDialog::QtSetGroupsDialog(ContactRosterItem* contact, const QList<QString>& allGroups) : contact_(contact) {
	//resize(300,300);
	setWindowTitle("Edit contact");
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	setContentsMargins(0,0,0,0);
	QScrollArea* scrollArea = new QScrollArea(this);
	layout->addWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QWidget* scroll = new QWidget(scrollArea);
	scrollArea->setWidget(scroll);
	QBoxLayout* scrollLayout = new QBoxLayout(QBoxLayout::TopToBottom, scroll);
	QLabel* label = new QLabel(scroll);
	label->setText("Choose groups for " + P2QSTRING(contact->getDisplayName()));
	scrollLayout->addWidget(label);
	foreach (QString group, allGroups) {
			QCheckBox* check = new QCheckBox(scroll);
			check->setText(group);
			check->setCheckState(Qt::Unchecked);
			checkBoxes_[Q2PSTRING(group)] = check;
			scrollLayout->addWidget(check);
		}
	foreach (String group, contact->getGroups()) {
		checkBoxes_[group]->setCheckState(Qt::Checked);
	}
	QWidget* newGroupWidget = new QWidget(scroll);
	QBoxLayout* newGroupLayout = new QBoxLayout(QBoxLayout::LeftToRight, newGroupWidget);
	scrollLayout->addWidget(newGroupWidget);
	newGroup_ = new QCheckBox(newGroupWidget);
	newGroup_->setText("New Group:");
	newGroup_->setCheckState(Qt::Unchecked);
	newGroupLayout->addWidget(newGroup_);
	newGroupName_ = new QLineEdit(newGroupWidget);
	newGroupLayout->addWidget(newGroupName_);
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	layout->addWidget(buttons);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

QtSetGroupsDialog::~QtSetGroupsDialog() {
	// TODO Auto-generated destructor stub
}

typedef std::pair<String, QCheckBox*> CheckStringPair;

boost::shared_ptr<RegroupRosterItemUIEvent> QtSetGroupsDialog::getRegroupEvent() {
	std::vector<String> addedGroups;
	std::vector<String> removedGroups;
	std::vector<String> existingGroups = contact_->getGroups();
	int tickedCount = 0;
	bool wantsContacts = false;
	foreach (CheckStringPair pair, checkBoxes_) {
		bool existing = std::find(existingGroups.begin(), existingGroups.end(), pair.first) != existingGroups.end();
		if (pair.second->checkState() == Qt::Checked) {
			tickedCount++;
			if (pair.first == "Contacts") {
				wantsContacts = true;
			}
			if (!existing && pair.first != "Contacts") {
				addedGroups.push_back(pair.first);
			}
		} else {
			if (existing) {
				removedGroups.push_back(pair.first);
			}
		}
	}
	if (newGroup_->checkState() == Qt::Checked) {
		tickedCount++;
		String name = Q2PSTRING(newGroupName_->text());
		if (std::find(existingGroups.begin(), existingGroups.end(), name) == existingGroups.end()) {
			addedGroups.push_back(name);
		}
	}
	if (tickedCount > 1 && wantsContacts) {
		addedGroups.push_back("Contacts");
	}
	boost::shared_ptr<RegroupRosterItemUIEvent> result(new RegroupRosterItemUIEvent(contact_->getJID(), addedGroups, removedGroups));
	return result;
}

}
