/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>

#include <QTreeView>

#include <Swift/Controllers/Contact.h>
#include <Swiften/Base/Log.h>

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

namespace Swift {

class ContactListDelegate;
class ContactListModel;
class SettingsProvider;
class QtRemovableItemDelegate;

class QtContactListWidget : public QTreeView {
	Q_OBJECT
public:
	QtContactListWidget(QWidget* parent, SettingsProvider* settings);
	virtual ~QtContactListWidget();

	void setList(const std::vector<Contact>& list);
	std::vector<Contact> getList() const;
	void setMaximumNoOfContactsToOne(bool limited);

public slots:
	void updateContacts(const std::vector<Contact>& contactUpdates);

signals:
	void onListChanged(std::vector<Contact> list);
	void onJIDsAdded(const std::vector<JID>& jids);

private slots:
	void handleListChanged(std::vector<Contact> list);

private:
	void handleSettingsChanged(const std::string&);

private:
	SettingsProvider* settings_;
	ContactListModel* contactListModel_;
	ContactListDelegate* contactListDelegate_;
	QtRemovableItemDelegate* removableItemDelegate_;
	bool limited_;
};

}
