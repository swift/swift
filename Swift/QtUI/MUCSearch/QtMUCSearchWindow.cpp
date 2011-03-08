/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/MUCSearch/QtMUCSearchWindow.h"

#include <qdebug.h>
#include <QMovie>
#include <QScrollBar>
#include <QTimer>
#include <QPushButton>

#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h"
#include "Swift/QtUI/MUCSearch/MUCSearchModel.h"
#include "Swift/QtUI/MUCSearch/MUCSearchDelegate.h"
#include "Swift/QtUI/MUCSearch/MUCSearchEmptyItem.h"
#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtMUCSearchWindow::QtMUCSearchWindow() {
	ui_.setupUi(this);
#ifndef Q_WS_MAC
	setWindowIcon(QIcon(":/logo-icon-16.png"));
#endif
	setModal(true);
	ui_.filter_->hide();
	model_ = new MUCSearchModel();
	delegate_ = new MUCSearchDelegate();
	ui_.results_->setModel(model_);
	ui_.results_->setItemDelegate(delegate_);
	ui_.results_->setHeaderHidden(true);
	ui_.results_->setRootIsDecorated(true);
	ui_.results_->setAnimated(true);
	ui_.results_->setAlternatingRowColors(true);
	connect(ui_.searchButton, SIGNAL(clicked()), this, SLOT(handleSearch()));
	connect(ui_.service_, SIGNAL(activated(const QString&)), this, SLOT(handleSearch(const QString&)));
	connect(ui_.results_->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)), this, SLOT(handleSelectionChanged (const QItemSelection&, const QItemSelection&)));
	connect(ui_.results_, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleActivated(const QModelIndex&)));
	connect(ui_.results_, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleActivated(const QModelIndex&)));
	// Not using a button box, because i can't seem to be able to make the ok button non-default (on mac)
	connect(ui_.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	ui_.okButton->setEnabled(false);
	connect(ui_.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	throbber_ = new QLabel(tr("Searching"), ui_.results_);
	throbber_->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), throbber_));
	throbber_->setToolTip(tr("Searching"));

	hasHadScrollBars_ = false;
	updateThrobberPosition();
	setSearchInProgress(false);
}

QtMUCSearchWindow::~QtMUCSearchWindow() {

}

void QtMUCSearchWindow::resizeEvent(QResizeEvent* /*event*/) {
	updateThrobberPosition();
}

void QtMUCSearchWindow::updateThrobberPosition() {
	bool isShown = throbber_->isVisible();
	int resultWidth = ui_.results_->width();
	int resultHeight = ui_.results_->height();
	//throbberWidth = throbber_->movie()->scaledSize().width();
	//throbberHeight = throbber_->movie()->scaledSize().height();
	int throbberWidth = 16; /* This is nasty, but the above doesn't work! */
	int throbberHeight = 16;
	/* It's difficult (or I spent a while trying) to work out whether the scrollbars are currently shown and their appropriate size,
	 * because if you listen for the expanded/collapsed signals, you seem to get them before the scrollbars are updated.
	 * This seems an acceptable workaround.
	 */
	hasHadScrollBars_ |= ui_.results_->verticalScrollBar()->isVisible();
	int hMargin = hasHadScrollBars_ ? ui_.results_->verticalScrollBar()->width() + 2 : 2;
	int vMargin = 2; /* We don't get horizontal scrollbars */
	throbber_->setGeometry(QRect(resultWidth - throbberWidth - hMargin, resultHeight - throbberHeight - vMargin, throbberWidth, throbberHeight)); /* include margins */
	throbber_->setVisible(isShown);
}

void QtMUCSearchWindow::addSavedServices(const std::list<JID>& services) {
	ui_.service_->clear();
	foreach (const JID& jid, services) {
		ui_.service_->addItem(P2QSTRING(jid.toString()));
	}
	if (!services.empty()) {
		ui_.service_->setEditText(P2QSTRING(services.begin()->toString()));
	}
	else {
		ui_.service_->clearEditText();
	}
}

void QtMUCSearchWindow::handleActivated(const QModelIndex& index) {
	if (!index.isValid()) {
		return;
	}
	if (dynamic_cast<MUCSearchRoomItem*>(static_cast<MUCSearchItem*>(index.internalPointer()))) {
		accept();
	}
}

void QtMUCSearchWindow::handleSearch() {
	handleSearch(ui_.service_->currentText());
}

void QtMUCSearchWindow::handleSearch(const QString& service) {
	if (!service.isEmpty()) {
		onSearchService(JID(Q2PSTRING(service)));
	}
}

void QtMUCSearchWindow::show() {
	QWidget::show();
	QWidget::activateWindow();
}

void QtMUCSearchWindow::clearList() {
	model_->clear();
}

void QtMUCSearchWindow::addService(const MUCService& service) {
	updateThrobberPosition();
	MUCSearchServiceItem* serviceItem = new MUCSearchServiceItem(P2QSTRING(service.getJID().toString()));
	if (service.getRooms().size() > 0) {
		foreach (MUCService::MUCRoom room, service.getRooms()) {
			new MUCSearchRoomItem(P2QSTRING(room.getNode()), serviceItem);
		}
	}
	else {
		new MUCSearchEmptyItem(serviceItem);
	}
	model_->addService(serviceItem);
	ui_.results_->expandAll();

}

void QtMUCSearchWindow::setSearchInProgress(bool searching) {
	if (searching) {
		throbber_->movie()->start();
	} else {
		throbber_->movie()->stop();
	}
	throbber_->setVisible(searching);
}

void QtMUCSearchWindow::accept() {
	MUCSearchRoomItem* room = getSelectedRoom();
	if (room) {
		onFinished(JID(Q2PSTRING(room->getNode()), Q2PSTRING(room->getParent()->getHost())));
	}
	else {
		onFinished(boost::optional<JID>());
	}
	QDialog::accept();
}

void QtMUCSearchWindow::reject() {
	onFinished(boost::optional<JID>());
	QDialog::reject();
}

void QtMUCSearchWindow::handleSelectionChanged(const QItemSelection&, const QItemSelection&) {
	ui_.okButton->setEnabled(getSelectedRoom());
}

MUCSearchRoomItem* QtMUCSearchWindow::getSelectedRoom() const {
	// Not using selectedIndexes(), because this seems to cause a crash in Qt (4.7.0) in the
	// QModelIndexList destructor.
	// This is a workaround posted in http://www.qtcentre.org/threads/16933 (although this case
	// was resolved by linking against the debug libs, ours isn't, and we're not alone)
	QItemSelection ranges = ui_.results_->selectionModel()->selection();
	QModelIndexList lstIndex;
	for (int i = 0; i < ranges.count(); ++i) {
		QModelIndex parent = ranges.at(i).parent();
		int right = ranges.at(i).model()->columnCount(parent) - 1;
		if (ranges.at(i).left() == 0 && ranges.at(i).right() == right) {
			for (int r = ranges.at(i).top(); r <= ranges.at(i).bottom(); ++r) {
				lstIndex.append(ranges.at(i).model()->index(r, 0, parent));
			}
		}
	}
	if (lstIndex.isEmpty()) {
		return NULL;
	}
	else {
		return dynamic_cast<MUCSearchRoomItem*>(static_cast<MUCSearchItem*>(lstIndex.first().internalPointer()));
	}
}

}
