/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtContactEditWidget.h"

#include <algorithm>

#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QMovie>
#include <QScrollArea>
#include <QRadioButton>

#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtContactEditWidget::QtContactEditWidget(const std::set<std::string>& allGroups, QWidget* parent) : QWidget(parent), nameRadioButton_(NULL), groups_(NULL) {
	QBoxLayout* layout = new QVBoxLayout(this);
	setContentsMargins(0,0,0,0);
	layout->setContentsMargins(0,0,0,0);

	nameLayout_ = new QHBoxLayout();
	suggestionsLayout_ = new QHBoxLayout();
	nameLayout_->addLayout(suggestionsLayout_);

	name_ = new QLineEdit(this);
	nameLayout_->addWidget(name_);

	throbberLabel_ = new QLabel(this);
	throbberLabel_->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), this));
	throbberLabel_->movie()->start();
	nameLayout_->addWidget(throbberLabel_);

	layout->addLayout(nameLayout_);

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
	std::string name;
	QList<QRadioButton*> buttons = findChildren<QRadioButton*>();
	foreach(const QRadioButton* button, buttons) {
		if (button->isChecked()) {
			if (button == nameRadioButton_) {
				name = Q2PSTRING(name_->text());
			} else {
				name = Q2PSTRING(button->text());
			}
			break;
		}
	}
	return name;
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

void QtContactEditWidget::setNameSuggestions(const std::vector<std::string>& suggestions) {
	throbberLabel_->movie()->stop();
	throbberLabel_->hide();

	foreach(const std::string& name, suggestions) {
		suggestionsLayout_->insertWidget(nameLayout_->count() - 2, new QRadioButton(P2QSTRING(name), this));
	}

	nameRadioButton_ = new QRadioButton(tr("Name:"), this);
	suggestionsLayout_->insertWidget(nameLayout_->count(), nameRadioButton_);

	if (name_->text().isEmpty()) {
		QRadioButton* suggestedRadioButton = dynamic_cast<QRadioButton*>(suggestionsLayout_->itemAt(0)->widget());
		if (suggestedRadioButton) {
			suggestedRadioButton->setChecked(true);
		}
	} else {
		nameRadioButton_->setChecked(true);
	}
}

void QtContactEditWidget::clear() {
	name_->clear();
	setSelectedGroups(std::vector<std::string>());
	newGroup_->setChecked(false);
	newGroupName_->clear();
	throbberLabel_->movie()->start();
	throbberLabel_->show();

	// clear suggestions
	while(suggestionsLayout_->count() != 0) {
		QLayoutItem *layoutItem = suggestionsLayout_->takeAt(0);
		delete layoutItem->widget();
		delete layoutItem;
	}
	nameRadioButton_ = NULL;
}

}
