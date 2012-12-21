/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/HighlightEditorWidget.h>
#include <Swift/QtUI/ui_QtHighlightEditorWidget.h>

namespace Swift {

	class QtHighlightRulesItemModel;

	class QtHighlightEditorWidget : public QWidget, public HighlightEditorWidget {
		Q_OBJECT

		public:
			QtHighlightEditorWidget(QWidget* parent = NULL);
			virtual ~QtHighlightEditorWidget();

			void show();

			void setHighlightManager(HighlightManager* highlightManager);

		private slots:
			void onNewButtonClicked();
			void onDeleteButtonClicked();
			void onMoveUpButtonClicked();
			void onMoveDownButtonClicked();
			void onCurrentRowChanged(const QModelIndex&);

		private:
			virtual void closeEvent(QCloseEvent* event);

			void selectRow(int row);
			int getSelectedRow() const;

			Ui::QtHighlightEditorWidget ui_;
			QtHighlightRulesItemModel* itemModel_;
		};

}
