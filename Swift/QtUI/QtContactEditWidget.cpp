/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtContactEditWidget.h"

#include <algorithm>

#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtContactEditWidget::QtContactEditWidget(const std::set<std::string>& allGroups, QWidget* parent) : QWidget(parent), groups_(NULL) {
	QBoxLayout* layout = new QVBoxLayout(this);
	setContentsMargins(0,0,0,0);
	layout->setContentsMargins(0,0,0,0);

	QHBoxLayout* nameLayout = new QHBoxLayout();
	
	QLabel* label = new QLabel(tr("Name:"), this);
	nameLayout->addWidget(label);
	name_ = new QLineEdit(this);
	nameLayout->addWidget(name_);
	layout->addLayout(nameLayout);

	layout->addWidget(new QLabel(tr("Groups:"), this));

	QScrollArea* groupsArea = new QScrollArea(this);
	layout->addWidget(groupsArea);
	groupsArea->setWidgetResizable(true);
	groupsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	groupsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	QWidget* groups = new QWidget(groupsArea);
	groupsArea->setWidget(groups);
	QVBoxLayout* scrollLayout = new QVBoxLayout(groups);

	foreach (std::string group, allGroups) {
		QCheckBox* check = new QCheckBox(groups);
		check->setText(P2QSTRING(group));
		check->setCheckState(Qt::Unchecked);
		checkBoxes_[group] = check;
		scrollLayout->addWidget(check);
	}

	QHBoxLayout* newGroupLayout = new QHBoxLayout();
	newGroup_ = new QCheckBox(groups);
	newGroup_->setText(tr("New Group:"));
	newGroup_->setCheckState(Qt::Unchecked);
	newGroupLayout->addWidget(newGroup_);
	newGroupName_ = new QLineEdit(groups);
	newGroupLayout->addWidget(newGroupName_);
	scrollLayout->addLayout(newGroupLayout);

	scrollLayout->addItem(new QSpacerItem(20, 73, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void QtContactEditWidget::setName(const std::string& name) {
	name_->setText(P2QSTRING(name));
}

std::string QtContactEditWidget::getName() const {
	return Q2PSTRING(name_->text());
}

void QtContactEditWidget::setSelectedGroups(const std::vector<std::string>& groups) {
	foreach (std::string group, groups) {
		checkBoxes_[group]->setCheckState(Qt::Checked);
	}
}

std::set<std::string> QtContactEditWidget::getSelectedGroups() const {
	std::set<std::string> groups;
	foreach(const CheckBoxMap::value_type& group, checkBoxes_) {
		if (group.second->checkState() == Qt::Checked) {
			groups.insert(group.first);
		}
	}
	if (newGroup_->checkState() == Qt::Checked && !newGroupName_->text().isEmpty()) {
		groups.insert(Q2PSTRING(newGroupName_->text()));
	}
	return groups;
}

void QtContactEditWidget::clear() {
	name_->clear();
	setSelectedGroups(std::vector<std::string>());
	newGroup_->setChecked(false);
	newGroupName_->clear();
}

}
