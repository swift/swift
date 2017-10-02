/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QCloseEvent>
#include <QPushButton>
#include <QShowEvent>
#include <QTreeView>

#include <Swift/Controllers/UIInterfaces/FileTransferListWidget.h>

#include <Swift/QtUI/QtTabbable.h>

namespace Swift {

class FileTransferOverview;
class QtFileTransferListItemModel;

class QtFileTransferListWidget : public QtTabbable, public FileTransferListWidget {
    Q_OBJECT

public:
    QtFileTransferListWidget();
    virtual ~QtFileTransferListWidget();

    void show();
    void activate();

    void setFileTransferOverview(FileTransferOverview *);

    virtual std::string getID() const;

private:
    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent* event);
    void handleFileTransferListChanged();

private slots:
    void clearInactiveTransfers();

private:
    QTreeView* treeView;

    QtFileTransferListItemModel* itemModel;
    FileTransferOverview* fileTransferOverview;
    QPushButton* clearFinished;
};

}
