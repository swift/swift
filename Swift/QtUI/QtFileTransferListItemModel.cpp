/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtFileTransferListItemModel.h"

#include <boost/bind.hpp>
#include <boost/cstdint.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swift/Controllers/FileTransfer/FileTransferController.h>
#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>

namespace Swift {

extern std::string formatSize(const boost::uintmax_t bytes);

QtFileTransferListItemModel::QtFileTransferListItemModel(QObject *parent) : QAbstractItemModel(parent), fileTransferOverview(0) {
}

void QtFileTransferListItemModel::setFileTransferOverview(FileTransferOverview *overview) {
	fileTransferOverview = overview;
	fileTransferOverview->onNewFileTransferController.connect(boost::bind(&QtFileTransferListItemModel::handleNewFileTransferController, this, _1));
}

void QtFileTransferListItemModel::handleNewFileTransferController(FileTransferController* newController) {
	emit layoutAboutToBeChanged();
	emit layoutChanged();
	dataChanged(createIndex(0,0), createIndex(fileTransferOverview->getFileTransfers().size(),4));
	newController->onStateChage.connect(boost::bind(&QtFileTransferListItemModel::handleStateChange, this, fileTransferOverview->getFileTransfers().size() - 1));
	newController->onProgressChange.connect(boost::bind(&QtFileTransferListItemModel::handleProgressChange, this, fileTransferOverview->getFileTransfers().size() - 1));
}

void QtFileTransferListItemModel::handleStateChange(int index) {
	emit dataChanged(createIndex(index, 2), createIndex(index, 2));
}

void QtFileTransferListItemModel::handleProgressChange(int index) {
	emit dataChanged(createIndex(index, 3), createIndex(index, 3));
}

QVariant QtFileTransferListItemModel::headerData(int section, Qt::Orientation /* orientation */, int role) const {
	if (role != Qt::DisplayRole) return QVariant();
	if (section == Direction) return QVariant("Direction");
	if (section == OtherParty) return QVariant("Other Party");
	if (section == State) return QVariant("State");
	if (section == Progress) return QVariant("Progress");
	if (section == OverallSize) return QVariant("Size");
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
		return controller->isIncoming() ? QVariant("Incoming") : QVariant("Outgoing");
	}
	if (index.column() == OtherParty) {
		return QVariant(QString::fromStdString(controller->getOtherParty().toString()));
	}
	if (index.column() == State) {
		FileTransfer::State state = controller->getState();
		switch(state.state) {
			case FileTransfer::State::WaitingForStart:
				return QVariant("Waiting for start");
			case FileTransfer::State::WaitingForAccept:
				return QVariant("Waiting for other side to accept");
			case FileTransfer::State::Negotiating:
				return QVariant("Negotiating");
			case FileTransfer::State::Transferring:
				return QVariant("Transferring");
			case FileTransfer::State::Finished:
				return QVariant("Finished");
			case FileTransfer::State::Failed:
				return QVariant("Failed");
			case FileTransfer::State::Canceled:
				return QVariant("Canceled");
		}
	}

	if (index.column() == Progress) {
		return QVariant(QString::number(controller->getProgress()));
	}
	if (index.column() == OverallSize) {
		return QVariant(QString::fromStdString(formatSize((controller->getSize()))));
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
	return createIndex(row, column, 0);
}

}
