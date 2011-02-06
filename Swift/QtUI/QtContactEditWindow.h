/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <boost/shared_ptr.hpp>

#include <QWidget>

#include <Swift/Controllers/UIInterfaces/ContactEditWindow.h>
#include <Swiften/Base/String.h>
#include <Swiften/JID/JID.h>

class QScrollArea;
class QLabel;
class QLineEdit;
class QCheckBox;

namespace Swift {
	class QtContactEditWindow : public QWidget, public ContactEditWindow {
			Q_OBJECT

		public:
			QtContactEditWindow();

			virtual void setContact(const JID& jid, const String& name, const std::vector<String>& groups, const std::set<String>& allGroups);

			void setEnabled(bool);
			void show();
			void hide();

		private slots:
			void handleRemoveContact();
			void handleUpdateContact();

		private:
			typedef std::map<String, QCheckBox*> CheckBoxMap;
			JID jid_;
			QLabel* jidLabel_;
			CheckBoxMap checkBoxes_;
			QLineEdit* name_;
			QScrollArea* groupsArea_;
			QWidget* groups_;
			QCheckBox* newGroup_;
			QLineEdit* newGroupName_;
	};
}

