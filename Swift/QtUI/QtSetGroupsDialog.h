/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <boost/shared_ptr.hpp>

#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QList>

#include "Swiften/Roster/ContactRosterItem.h"
#include "Swift/Controllers/UIEvents/RegroupRosterItemUIEvent.h"

namespace Swift {

class QtSetGroupsDialog : public QDialog {
	Q_OBJECT
	public:
		QtSetGroupsDialog(ContactRosterItem* contact, const QList<QString>& allGroups);
		virtual ~QtSetGroupsDialog();
		boost::shared_ptr<RegroupRosterItemUIEvent> getRegroupEvent();
	private:
		ContactRosterItem* contact_;
		std::map<String, QCheckBox*> checkBoxes_;
		QCheckBox* newGroup_;
		QLineEdit* newGroupName_;
};

}

