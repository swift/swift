/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtContactEditWindow.h"

#include <algorithm>

#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtContactEditWindow::QtContactEditWindow() : groups_(NULL) {
	resize(300,300);
	setWindowTitle("Edit contact");

	QBoxLayout* layout = new QVBoxLayout(this);
	setContentsMargins(0,0,0,0);

	jidLabel_ = new QLabel(this);
	jidLabel_->setAlignment(Qt::AlignHCenter);
	layout->addWidget(jidLabel_);

	QHBoxLayout* nameLayout = new QHBoxLayout();
	
	QLabel* label = new QLabel("Name:", this);
	nameLayout->addWidget(label);
	name_ = new QLineEdit(this);
	nameLayout->addWidget(name_);
	layout->addLayout(nameLayout);

	layout->addWidget(new QLabel("Groups:", this));

	groupsArea_ = new QScrollArea(this);
	layout->addWidget(groupsArea_);
	groupsArea_->setWidgetResizable(true);
	groupsArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	groupsArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	layout->addLayout(buttonLayout);
	QPushButton* removeButton = new QPushButton("Remove contact", this);
	connect(removeButton, SIGNAL(clicked()), this, SLOT(handleRemoveContact()));
	buttonLayout->addWidget(removeButton);
	QPushButton* okButton = new QPushButton("Ok", this);
	connect(okButton, SIGNAL(clicked()), this, SLOT(handleUpdateContact()));
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton);
}

void QtContactEditWindow::setContact(const JID& jid, const String& name, const std::vector<String>& groups, const std::set<String>& allGroups) {
	jid_ = jid;
	
	jidLabel_->setText("<b>" + P2QSTRING(jid.toString()) + "</b>");
	name_->setText(P2QSTRING(name));

	delete groups_;
	checkBoxes_.clear();
	groups_ = new QWidget(groupsArea_);
	groupsArea_->setWidget(groups_);
	QVBoxLayout* scrollLayout = new QVBoxLayout(groups_);

	foreach (String group, allGroups) {
		QCheckBox* check = new QCheckBox(groups_);
		check->setText(P2QSTRING(group));
		check->setCheckState(Qt::Unchecked);
		checkBoxes_[group] = check;
		scrollLayout->addWidget(check);
	}
	foreach (String group, groups) {
		checkBoxes_[group]->setCheckState(Qt::Checked);
	}

	QHBoxLayout* newGroupLayout = new QHBoxLayout();
	newGroup_ = new QCheckBox(groups_);
	newGroup_->setText("New Group:");
	newGroup_->setCheckState(Qt::Unchecked);
	newGroupLayout->addWidget(newGroup_);
	newGroupName_ = new QLineEdit(groups_);
	newGroupLayout->addWidget(newGroupName_);
	scrollLayout->addLayout(newGroupLayout);

	scrollLayout->addItem(new QSpacerItem(20, 73, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void QtContactEditWindow::setEnabled(bool b) {
	QWidget::setEnabled(b);
}

void QtContactEditWindow::show() {
	QWidget::show();
	QWidget::activateWindow();
}

void QtContactEditWindow::hide() {
	QWidget::hide();
}

void QtContactEditWindow::handleRemoveContact() {
	QMessageBox msgBox;
	msgBox.setWindowTitle("Confirm contact deletion");
	msgBox.setText("Are you sure you want to delete this contact?");
	msgBox.setInformativeText(QString("This will remove the contact '%1' from all groups they may be in.").arg(P2QSTRING(jid_.toString())));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::Yes);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes) {
		onRemoveContactRequest();
	}
}

void QtContactEditWindow::handleUpdateContact() {
	std::vector<String> groups;
	foreach(const CheckBoxMap::value_type& group, checkBoxes_) {
		if (group.second->checkState() == Qt::Checked) {
			groups.push_back(group.first);
		}
	}
	if (newGroup_->checkState() == Qt::Checked && !newGroupName_->text().isEmpty()) {
		groups.push_back(Q2PSTRING(newGroupName_->text()));
	}
	onChangeContactRequest(Q2PSTRING(name_->text()), groups);
}

}
