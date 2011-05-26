/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/FileTransferListWidget.h"

#include "QtTabbable.h"

#include <QCloseEvent>
#include <QShowEvent>
#include <QTreeView>

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

private:
	virtual void closeEvent(QCloseEvent* event);
	virtual void showEvent(QShowEvent* event);

private:
	QTreeView* treeView;

	QtFileTransferListItemModel* itemModel;
	FileTransferOverview* fileTransferOverview;
};

}
