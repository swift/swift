/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtContactEditWidget.h>

#include <algorithm>

#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QMovie>
#include <QRadioButton>
#include <QScrollArea>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtContactEditWidget::QtContactEditWidget(const std::set<std::string>& allGroups, QWidget* parent) : QWidget(parent), nameRadioButton_(nullptr), groups_(nullptr) {
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

    for (const auto& group : allGroups) {
        QString groupName = doubleAmpersand(group);
        QCheckBox* check = new QCheckBox(groups);
        check->setText(groupName);
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
    std::string name = Q2PSTRING(name_->text());
    QList<QRadioButton*> buttons = findChildren<QRadioButton*>();
    for (const auto button : buttons) {
        if (button->isChecked()) {
            if (button == nameRadioButton_) {
                name = Q2PSTRING(name_->text());
            } else {
                name = singleAmpersand(button->text());
            }
            break;
        }
    }
    return name;
}

void QtContactEditWidget::setSelectedGroups(const std::vector<std::string>& groups) {
    for  (auto&& group : groups) {
        checkBoxes_[group]->setCheckState(Qt::Checked);
    }
}

std::set<std::string> QtContactEditWidget::getSelectedGroups() const {
    std::set<std::string> groups;
    for (const auto& group : checkBoxes_) {
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

    // remove old suggestions except for the user input text field
    QLayoutItem* suggestionItem = nullptr;
    while ((suggestionItem = suggestionsLayout_->itemAt(0)) && suggestionItem->widget() != name_) {
        QWidget* suggestionWidget = suggestionItem->widget();
        suggestionsLayout_->removeWidget(suggestionWidget);
        delete suggestionWidget;
    }

    // populate new suggestions
    for (const auto& name : suggestions) {
        suggestionsLayout_->insertWidget(nameLayout_->count() - 2, new QRadioButton(doubleAmpersand(name), this));
    }

    nameRadioButton_ = new QRadioButton(tr("Name:"), this);
    suggestionsLayout_->insertWidget(nameLayout_->count(), nameRadioButton_);

    QRadioButton* suggestedRadioButton = nullptr;
    QList<QRadioButton*> radioButtons = findChildren<QRadioButton*>();
    for (auto candidate : radioButtons) {
        if (candidate->text() == name_->text()) {
            suggestedRadioButton = candidate;
            break;
        }
    }
    if (suggestedRadioButton) {
        suggestedRadioButton->setChecked(true);
    } else {
        nameRadioButton_->setChecked(true);
    }
}
QString QtContactEditWidget::doubleAmpersand(const std::string& name) const {
    return P2QSTRING(name).replace("&", "&&");

}

std::string QtContactEditWidget::singleAmpersand(const QString& name) const {
    return Q2PSTRING(QString(name).replace("&&", "&"));
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
    nameRadioButton_ = nullptr;
}

}
