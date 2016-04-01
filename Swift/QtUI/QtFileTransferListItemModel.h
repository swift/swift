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

#pragma once

#include <QAbstractItemModel>

namespace Swift {

class FileTransferController;
class FileTransferOverview;

class QtFileTransferListItemModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit QtFileTransferListItemModel(QObject *parent = nullptr);
    virtual ~QtFileTransferListItemModel();

    void setFileTransferOverview(FileTransferOverview*);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

private:
    enum Columns {
        Direction = 0,
        OtherParty,
        State,
        Progress,
        OverallSize,
        NoOfColumns
    };

private:
    void handleNewFileTransferController(FileTransferController*);
    void handleFileTransferListChanged();
    void handleStateChange(int index);
    void handleProgressChange(int index);

signals:

public slots:

private:
    FileTransferOverview *fileTransferOverview;

};

}
