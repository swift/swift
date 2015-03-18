/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/UserSearch/QtSuggestingJIDInput.h>
#include <Swift/QtUI/UserSearch/ContactListDelegate.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/UserSearch/ContactListModel.h>

#include <Swiften/Base/boost_bsignals.h>
#include <boost/bind.hpp>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <QAbstractItemView>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>


namespace Swift {

QtSuggestingJIDInput::QtSuggestingJIDInput(QWidget* parent, SettingsProvider* settings) : QLineEdit(parent), settings_(settings) {
	treeViewPopup_ = new QTreeView();
	treeViewPopup_->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
	//treeViewPopup_->setAttribute(Qt::WA_ShowWithoutActivating);
	treeViewPopup_->setAlternatingRowColors(true);
	treeViewPopup_->setIndentation(0);
	treeViewPopup_->setHeaderHidden(true);
	treeViewPopup_->setExpandsOnDoubleClick(false);
	treeViewPopup_->setItemsExpandable(false);
	treeViewPopup_->setSelectionMode(QAbstractItemView::SingleSelection);
	treeViewPopup_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	treeViewPopup_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QSizePolicy policy(treeViewPopup_->sizePolicy());
	policy.setVerticalPolicy(QSizePolicy::Expanding);
	treeViewPopup_->setSizePolicy(policy);
	treeViewPopup_->hide();
	treeViewPopup_->setFocusProxy(this);
	connect(treeViewPopup_, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
	connect(treeViewPopup_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));

	contactListModel_ = new ContactListModel(false);
	treeViewPopup_->setModel(contactListModel_);

	contactListDelegate_ = new ContactListDelegate(settings->getSetting(QtUISettingConstants::COMPACT_ROSTER));
	treeViewPopup_->setItemDelegate(contactListDelegate_);

	settings_->onSettingChanged.connect(boost::bind(&QtSuggestingJIDInput::handleSettingsChanged, this, _1));
}

QtSuggestingJIDInput::~QtSuggestingJIDInput() {
	settings_->onSettingChanged.disconnect(boost::bind(&QtSuggestingJIDInput::handleSettingsChanged, this, _1));
	delete treeViewPopup_;
	delete contactListDelegate_;
	delete contactListModel_;
}

Contact::ref QtSuggestingJIDInput::getContact() {
	if (!!currentContact_) {
		return currentContact_;
	}

	if (!text().isEmpty()) {
		JID jid(Q2PSTRING(text()));
		if (jid.isValid()) {
			Contact::ref manualContact = boost::make_shared<Contact>();
			manualContact->name = jid.toString();
			manualContact->jid = jid;
			return manualContact;
		}
	}
	return boost::shared_ptr<Contact>();
}

void QtSuggestingJIDInput::setSuggestions(const std::vector<Contact::ref>& suggestions) {
	contactListModel_->setList(suggestions);
	positionPopup();
	if (!suggestions.empty()) {
		treeViewPopup_->setCurrentIndex(contactListModel_->index(0, 0));
		showPopup();
	} else {
		currentContact_.reset();
		hidePopup();
	}
}

void QtSuggestingJIDInput::clear() {
	setText("");
	currentContact_.reset();
}

void QtSuggestingJIDInput::keyPressEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key_Up) {
		if (contactListModel_->rowCount() > 0) {
			int row = treeViewPopup_->currentIndex().row();
			row = (row + contactListModel_->rowCount() - 1) % contactListModel_->rowCount();
			treeViewPopup_->setCurrentIndex(contactListModel_->index(row, 0));
		}
	} else if (event->key() == Qt::Key_Down) {
		if (contactListModel_->rowCount() > 0) {
			int row = treeViewPopup_->currentIndex().row();
			row = (row + contactListModel_->rowCount() + 1) % contactListModel_->rowCount();
			treeViewPopup_->setCurrentIndex(contactListModel_->index(row, 0));
		}
	} else if (event->key() == Qt::Key_Return && treeViewPopup_->isVisible()) {
		QModelIndex index = treeViewPopup_->currentIndex();
		if (!contactListModel_->getList().empty() && index.isValid()) {
			currentContact_ = contactListModel_->getContact(index.row());
			if (currentContact_->jid.isValid()) {
				setText(P2QSTRING(currentContact_->jid.toString()));
			} else {
				setText(P2QSTRING(currentContact_->name));
			}
			hidePopup();
			clearFocus();
		} else {
			currentContact_.reset();
		}
		editingDone();
	} else {
		QLineEdit::keyPressEvent(event);
	}
}

void QtSuggestingJIDInput::hideEvent(QHideEvent* /* event */) {
	// Hide our popup when we are hidden (can happen when a dialog is closed by the user).
	treeViewPopup_->hide();
}

void QtSuggestingJIDInput::handleApplicationFocusChanged(QWidget* /*old*/, QWidget* /*now*/) {
	/* Using the now argument gives use the wrong widget. This is part of the code needed
	   to prevent stealing of focus when opening a the suggestion window. */
	QWidget* now = qApp->focusWidget();
	if (!now || (now != treeViewPopup_ && now != this && !now->isAncestorOf(this) && !now->isAncestorOf(treeViewPopup_) && !this->isAncestorOf(now) && !treeViewPopup_->isAncestorOf(now))) {
		hidePopup();
	}
}

void QtSuggestingJIDInput::handleSettingsChanged(const std::string& setting) {
	if (setting == QtUISettingConstants::COMPACT_ROSTER.getKey()) {
		contactListDelegate_->setCompact(settings_->getSetting(QtUISettingConstants::COMPACT_ROSTER));
	}
}

void QtSuggestingJIDInput::handleClicked(const QModelIndex& index) {
	if (index.isValid()) {
		currentContact_ = contactListModel_->getContact(index.row());
		onUserSelected(currentContact_);
		hidePopup();
	}
}

void QtSuggestingJIDInput::positionPopup() {
	QDesktopWidget* desktop = QApplication::desktop();
	int screen = desktop->screenNumber(this);
	QPoint point = mapToGlobal(QPoint(0, height()));
	QRect geometry = desktop->availableGeometry(screen);
	int x = point.x();
	int y = point.y();
	int width = this->width();
	int height = 80;

	int screenWidth = geometry.x() + geometry.width();
	if (x + width > screenWidth) {
		x = screenWidth - width;
	}

	height = treeViewPopup_->sizeHintForRow(0) * contactListModel_->rowCount();
	height = height > 200 ? 200 : height;

	int marginLeft;
	int marginTop;
	int marginRight;
	int marginBottom;
	treeViewPopup_->getContentsMargins(&marginLeft, &marginTop, &marginRight, &marginBottom);
	height += marginTop + marginBottom;
	width += marginLeft + marginRight;

	treeViewPopup_->setGeometry(x, y, width, height);
	treeViewPopup_->move(x, y);
	treeViewPopup_->setMaximumWidth(width);
}

void QtSuggestingJIDInput::showPopup() {
	treeViewPopup_->show();
	activateWindow();
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(handleApplicationFocusChanged(QWidget*, QWidget*)), Qt::QueuedConnection);
	setFocus();
}

void QtSuggestingJIDInput::hidePopup() {
	disconnect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(handleApplicationFocusChanged(QWidget*, QWidget*)));
	treeViewPopup_->hide();

	// Give focus back to input widget because the hide() call passes the focus to the wrong widget.
	setFocus();
}

}
