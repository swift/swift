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

#include <Swift/QtUI/QtFileTransferListItemModel.h>

#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/FileSize.h>

#include <Swift/Controllers/FileTransfer/FileTransferController.h>
#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtFileTransferListItemModel::QtFileTransferListItemModel(QObject *parent) : QAbstractItemModel(parent), fileTransferOverview(nullptr) {
}

QtFileTransferListItemModel::~QtFileTransferListItemModel() {
    if (fileTransferOverview) {
        fileTransferOverview->onNewFileTransferController.disconnect(boost::bind(&QtFileTransferListItemModel::handleNewFileTransferController, this, _1));
        fileTransferOverview->onFileTransferListChanged.disconnect(boost::bind(&QtFileTransferListItemModel::handleFileTransferListChanged, this));
    }
}

void QtFileTransferListItemModel::setFileTransferOverview(FileTransferOverview *overview) {
    if (fileTransferOverview) {
        fileTransferOverview->onNewFileTransferController.disconnect(boost::bind(&QtFileTransferListItemModel::handleNewFileTransferController, this, _1));
        fileTransferOverview->onFileTransferListChanged.disconnect(boost::bind(&QtFileTransferListItemModel::handleFileTransferListChanged, this));
    }
    fileTransferOverview = overview;
    if (fileTransferOverview) {
        fileTransferOverview->onNewFileTransferController.connect(boost::bind(&QtFileTransferListItemModel::handleNewFileTransferController, this, _1));
        fileTransferOverview->onFileTransferListChanged.connect(boost::bind(&QtFileTransferListItemModel::handleFileTransferListChanged, this));
    }
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void QtFileTransferListItemModel::handleNewFileTransferController(FileTransferController* newController) {
    emit layoutAboutToBeChanged();
    emit layoutChanged();
    dataChanged(createIndex(0,0), createIndex(fileTransferOverview->getFileTransfers().size(),4));
    newController->onStateChanged.connect(boost::bind(&QtFileTransferListItemModel::handleStateChange, this, fileTransferOverview->getFileTransfers().size() - 1));
    newController->onProgressChange.connect(boost::bind(&QtFileTransferListItemModel::handleProgressChange, this, fileTransferOverview->getFileTransfers().size() - 1));
}

void QtFileTransferListItemModel::handleFileTransferListChanged() {
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void QtFileTransferListItemModel::handleStateChange(int index) {
    emit dataChanged(createIndex(index, 2), createIndex(index, 2));
}

void QtFileTransferListItemModel::handleProgressChange(int index) {
    emit dataChanged(createIndex(index, 3), createIndex(index, 3));
}

QVariant QtFileTransferListItemModel::headerData(int section, Qt::Orientation /* orientation */, int role) const {
    if (role != Qt::DisplayRole) return QVariant();
    if (section == Direction) return QVariant(QObject::tr("Direction"));
    if (section == OtherParty) return QVariant(QObject::tr("Other Party"));
    if (section == State) return QVariant(QObject::tr("State"));
    if (section == Progress) return QVariant(QObject::tr("Progress"));
    if (section == OverallSize) return QVariant(QObject::tr("Size"));
    return QVariant();
}

int QtFileTransferListItemModel::columnCount(const QModelIndex& /* parent */) const {
    return NoOfColumns;
}

QVariant QtFileTransferListItemModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole || !index.isValid() ||
        !fileTransferOverview || static_cast<size_t>(index.row()) >= fileTransferOverview->getFileTransfers().size()) {
        return QVariant();
    }
    FileTransferController* controller = fileTransferOverview->getFileTransfers().at(index.row());
    if (index.column() == Direction) {
        return controller->isIncoming() ? QVariant(QObject::tr("Incoming")) : QVariant(QObject::tr("Outgoing"));
    }
    if (index.column() == OtherParty) {
        return QVariant(P2QSTRING(controller->getOtherParty().toString()));
    }
    if (index.column() == State) {
        FileTransfer::State state = controller->getState();
        switch(state.type) {
            case FileTransfer::State::Initial:
                assert(false);
                return QVariant("");
            case FileTransfer::State::WaitingForStart:
                return QVariant(QObject::tr("Waiting for start"));
            case FileTransfer::State::WaitingForAccept:
                return QVariant(QObject::tr("Waiting for other side to accept"));
            case FileTransfer::State::Negotiating:
                return QVariant(QObject::tr("Negotiating"));
            case FileTransfer::State::Transferring:
                return QVariant(QObject::tr("Transferring"));
            case FileTransfer::State::Finished:
                return QVariant(QObject::tr("Finished"));
            case FileTransfer::State::Failed:
                return QVariant(QObject::tr("Failed"));
            case FileTransfer::State::Canceled:
                return QVariant(QObject::tr("Canceled"));
        }
    }

    if (index.column() == Progress) {
        return QVariant(QString::number(controller->getProgress()));
    }
    if (index.column() == OverallSize) {
        return QVariant(P2QSTRING(formatSize((controller->getSize()))));
    }
    return QVariant();
}

QModelIndex QtFileTransferListItemModel::parent(const QModelIndex& /* child */) const {
    return createIndex(0,0);
}

int QtFileTransferListItemModel::rowCount(const QModelIndex& /* parent */) const {
    return fileTransferOverview ? fileTransferOverview->getFileTransfers().size() : 0;
}

QModelIndex QtFileTransferListItemModel::index(int row, int column, const QModelIndex& /* parent */) const {
    return createIndex(row, column, static_cast<void*>(nullptr));
}

}
