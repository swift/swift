/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <QWidget>

class QLabel;
class QLineEdit;
class QCheckBox;
class QHBoxLayout;
class QRadioButton;

namespace Swift {
    class QtContactEditWidget : public QWidget {
            Q_OBJECT

        public:
            QtContactEditWidget(const std::set<std::string>& allGroups, QWidget* parent);

            void setName(const std::string&);
            std::string getName() const;

            void setSelectedGroups(const std::vector<std::string>& groups);
            std::set<std::string> getSelectedGroups() const;

            void setNameSuggestions(const std::vector<std::string>& suggestions);

            void clear();


        private:
            QString doubleAmpersand(const std::string& name) const;
            std::string singleAmpersand(const QString& name) const;

        private:
            std::map<std::string, QCheckBox*> checkBoxes_;
            QHBoxLayout* nameLayout_;
            QHBoxLayout* suggestionsLayout_;
            QRadioButton* nameRadioButton_;
            QLineEdit* name_;
            QWidget* groups_;
            QCheckBox* newGroup_;
            QLineEdit* newGroupName_;
            QLabel* throbberLabel_;
    };
}

