/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <set>
#include <boost/shared_ptr.hpp>

#include <QWidget>

#include <vector>
#include <string>

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
			typedef std::map<std::string, QCheckBox*> CheckBoxMap;
			CheckBoxMap checkBoxes_;
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

