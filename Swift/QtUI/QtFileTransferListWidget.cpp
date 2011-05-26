/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtFileTransferListWidget.h"

#include <Swift/QtUI/QtFileTransferListItemModel.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>

namespace Swift {

QtFileTransferListWidget::QtFileTransferListWidget() : fileTransferOverview(0) {
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);

	treeView = new QTreeView(this);
	treeView->setRootIsDecorated(false);
	treeView->setItemsExpandable(false);
	layout->addWidget(treeView);

	itemModel = new QtFileTransferListItemModel();
	treeView->setModel(itemModel);

	QWidget* bottom = new QWidget(this);
	layout->addWidget(bottom);
	bottom->setAutoFillBackground(true);

	QHBoxLayout* buttonLayout = new QHBoxLayout(bottom);
	buttonLayout->setContentsMargins(10,0,20,0);
	buttonLayout->setSpacing(0);

	QPushButton* clearFinished = new QPushButton(tr("Clear Finished Transfers"), bottom);
	clearFinished->setEnabled(false);
	//connect(clearButton, SIGNAL(clicked()), textEdit, SLOT(clear()));
	buttonLayout->addWidget(clearFinished);

	setWindowTitle(tr("File Transfer List"));
	emit titleUpdated();
}

QtFileTransferListWidget::~QtFileTransferListWidget() {
	delete itemModel;
}

void QtFileTransferListWidget::showEvent(QShowEvent* event) {
	emit windowOpening();
	emit titleUpdated(); /* This just needs to be somewhere after construction */
	QWidget::showEvent(event);
}

void QtFileTransferListWidget::show() {
	QWidget::show();
	emit windowOpening();
}

void QtFileTransferListWidget::activate() {
	emit wantsToActivate();
}

void QtFileTransferListWidget::setFileTransferOverview(FileTransferOverview *overview) {
	fileTransferOverview = overview;
	itemModel->setFileTransferOverview(overview);
}

void QtFileTransferListWidget::closeEvent(QCloseEvent* event) {
	emit windowClosing();
	event->accept();
}

}
