/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/BlockListEditorWidget.h>
#include <Swift/QtUI/QtVCardWidget/QtRemovableItemDelegate.h>

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
	class QtBlockListEditorWindow;
}

namespace Swift {

class QtBlockListEditorWindow : public QWidget, public BlockListEditorWidget {
	Q_OBJECT

	public:
		QtBlockListEditorWindow();
		virtual ~QtBlockListEditorWindow();

		virtual void show();
		virtual void setCurrentBlockList(const std::vector<JID>& blockedJIDs);
		virtual void setBusy(bool isBusy);
		virtual std::vector<JID> getCurrentBlockList() const;

	private slots:
		void handleItemChanged(QTreeWidgetItem*, int);
		void applyChanges();

	private:
		Ui::QtBlockListEditorWindow* ui;
		QtRemovableItemDelegate* itemDelegate;
};

}
