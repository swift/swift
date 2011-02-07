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

#include <Swiften/Base/String.h>

class QLineEdit;
class QCheckBox;

namespace Swift {
	class QtContactEditWidget : public QWidget {
			Q_OBJECT

		public:
			QtContactEditWidget(const std::set<String>& allGroups, QWidget* parent);

			void setName(const String&);
			String getName() const;

			void setSelectedGroups(const std::vector<String>& groups);
			std::set<String> getSelectedGroups() const;

		private:
			typedef std::map<String, QCheckBox*> CheckBoxMap;
			CheckBoxMap checkBoxes_;
			QLineEdit* name_;
			QWidget* groups_;
			QCheckBox* newGroup_;
			QLineEdit* newGroupName_;
	};
}

