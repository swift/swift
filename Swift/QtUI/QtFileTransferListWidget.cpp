/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtFileTransferListWidget.h>

#include <boost/bind.hpp>

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>

#include <Swift/QtUI/QtFileTransferListItemModel.h>

namespace Swift {

QtFileTransferListWidget::QtFileTransferListWidget() : fileTransferOverview(nullptr) {
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

    clearFinished = new QPushButton(tr("Clear all"), bottom);
    clearFinished->setEnabled(false);
    connect(clearFinished, SIGNAL(clicked()), this, SLOT(clearInactiveTransfers()));
    buttonLayout->addWidget(clearFinished);

    setWindowTitle(tr("File Transfer List"));
    emit titleUpdated();
}

QtFileTransferListWidget::~QtFileTransferListWidget() {
    if (fileTransferOverview) {
        fileTransferOverview->onFileTransferListChanged.disconnect(boost::bind(&QtFileTransferListWidget::handleFileTransferListChanged, this));
        fileTransferOverview = nullptr;
    }
    delete itemModel;
}

void QtFileTransferListWidget::showEvent(QShowEvent* event) {
    emit windowOpening();
    emit titleUpdated(); /* This just needs to be somewhere after construction */
    QWidget::showEvent(event);
}

void QtFileTransferListWidget::handleFileTransferListChanged() {
    clearFinished->setEnabled(fileTransferOverview->isClearable());
}

void QtFileTransferListWidget::clearInactiveTransfers() {
    fileTransferOverview->clearFinished();
}

void QtFileTransferListWidget::show() {
    QWidget::show();
    emit windowOpening();
}

void QtFileTransferListWidget::activate() {
    emit wantsToActivate();
}

void QtFileTransferListWidget::setFileTransferOverview(FileTransferOverview *overview) {
    if (fileTransferOverview) {
        fileTransferOverview->onFileTransferListChanged.disconnect(boost::bind(&QtFileTransferListWidget::handleFileTransferListChanged, this));
        fileTransferOverview = nullptr;
    }
    if (overview) {
        fileTransferOverview = overview;
        fileTransferOverview->onFileTransferListChanged.connect(boost::bind(&QtFileTransferListWidget::handleFileTransferListChanged, this));
        clearFinished->setEnabled(fileTransferOverview->isClearable());
    }
    itemModel->setFileTransferOverview(overview);
}

std::string QtFileTransferListWidget::getID() const {
    return "QtFileTransferListWidget";
}

void QtFileTransferListWidget::closeEvent(QCloseEvent* event) {
    emit windowClosing();
    event->accept();
}

}
