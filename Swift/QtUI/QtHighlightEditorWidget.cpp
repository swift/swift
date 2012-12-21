/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cassert>

#include <Swift/QtUI/QtHighlightEditorWidget.h>
#include <Swift/QtUI/QtHighlightRulesItemModel.h>

namespace Swift {

QtHighlightEditorWidget::QtHighlightEditorWidget(QWidget* parent)
	: QWidget(parent)
{
	ui_.setupUi(this);

	itemModel_ = new QtHighlightRulesItemModel(this);
	ui_.treeView->setModel(itemModel_);
	ui_.ruleWidget->setModel(itemModel_);

	for (int i = 0; i < QtHighlightRulesItemModel::NumberOfColumns; ++i) {
		switch (i) {
			case QtHighlightRulesItemModel::ApplyTo:
			case QtHighlightRulesItemModel::Sender:
			case QtHighlightRulesItemModel::Keyword:
			case QtHighlightRulesItemModel::Action:
				ui_.treeView->showColumn(i);
				break;
			default:
				ui_.treeView->hideColumn(i);
				break;
		}
	}

	setHighlightManager(NULL); // setup buttons for empty rules list

	connect(ui_.treeView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(onCurrentRowChanged(QModelIndex)));

	connect(ui_.newButton, SIGNAL(clicked()), SLOT(onNewButtonClicked()));
	connect(ui_.deleteButton, SIGNAL(clicked()), SLOT(onDeleteButtonClicked()));

	connect(ui_.moveUpButton, SIGNAL(clicked()), SLOT(onMoveUpButtonClicked()));
	connect(ui_.moveDownButton, SIGNAL(clicked()), SLOT(onMoveDownButtonClicked()));

	connect(ui_.closeButton, SIGNAL(clicked()), SLOT(close()));

	setWindowTitle(tr("Highlight Rules"));
}

QtHighlightEditorWidget::~QtHighlightEditorWidget()
{
}

void QtHighlightEditorWidget::show()
{
	if (itemModel_->rowCount(QModelIndex())) {
		selectRow(0);
	}
	QWidget::show();
	QWidget::activateWindow();
}

void QtHighlightEditorWidget::setHighlightManager(HighlightManager* highlightManager)
{
	itemModel_->setHighlightManager(highlightManager);
	ui_.newButton->setEnabled(highlightManager != NULL);

	ui_.ruleWidget->setEnabled(false);
	ui_.deleteButton->setEnabled(false);
	ui_.moveUpButton->setEnabled(false);
	ui_.moveDownButton->setEnabled(false);
}

void QtHighlightEditorWidget::closeEvent(QCloseEvent* event) {
	ui_.ruleWidget->save();
	event->accept();
}

void QtHighlightEditorWidget::onNewButtonClicked()
{
	int row = getSelectedRow() + 1;
	itemModel_->insertRow(row, QModelIndex());
	selectRow(row);
}

void QtHighlightEditorWidget::onDeleteButtonClicked()
{
	int row = getSelectedRow();
	assert(row >= 0);

	itemModel_->removeRow(row, QModelIndex());
	if (row == itemModel_->rowCount(QModelIndex())) {
		--row;
	}
	selectRow(row);
}

void QtHighlightEditorWidget::onMoveUpButtonClicked()
{
	int row = getSelectedRow();
	assert(row > 0);

	ui_.ruleWidget->save();
	ui_.ruleWidget->setActiveIndex(QModelIndex());
	itemModel_->swapRows(row, row - 1);
	selectRow(row - 1);
}

void QtHighlightEditorWidget::onMoveDownButtonClicked()
{
	int row = getSelectedRow();
	assert(row < itemModel_->rowCount(QModelIndex()) - 1);

	ui_.ruleWidget->save();
	ui_.ruleWidget->setActiveIndex(QModelIndex());
	if (itemModel_->swapRows(row, row + 1)) {
		selectRow(row + 1);
	}
}

void QtHighlightEditorWidget::onCurrentRowChanged(const QModelIndex& index)
{
	ui_.ruleWidget->save();
	ui_.ruleWidget->setActiveIndex(index);

	ui_.ruleWidget->setEnabled(index.isValid());

	ui_.deleteButton->setEnabled(index.isValid());

	ui_.moveUpButton->setEnabled(index.isValid() && index.row() != 0);
	ui_.moveDownButton->setEnabled(index.isValid() && index.row() != itemModel_->rowCount(QModelIndex()) - 1);
}

void QtHighlightEditorWidget::selectRow(int row)
{
	QModelIndex index = itemModel_->index(row, 0, QModelIndex());
	ui_.treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

/** Return index of selected row or -1 if none is selected */
int QtHighlightEditorWidget::getSelectedRow() const
{
	QModelIndexList rows = ui_.treeView->selectionModel()->selectedRows();
	return rows.isEmpty() ? -1 : rows[0].row();
}

}
