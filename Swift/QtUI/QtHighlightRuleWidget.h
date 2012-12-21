/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QWidget>
#include <QModelIndex>

#include <Swift/QtUI/ui_QtHighlightRuleWidget.h>

class QDataWidgetMapper;

namespace Swift {

	class QtHighlightRulesItemModel;

	class QtHighlightRuleWidget : public QWidget
	{
		Q_OBJECT

		public:
			explicit QtHighlightRuleWidget(QWidget* parent = NULL);
			~QtHighlightRuleWidget();

			void setModel(QtHighlightRulesItemModel* model);

		public slots:
			void setActiveIndex(const QModelIndex&);
			void save();
			void revert();

		private slots:
			void onHighlightTextToggled(bool);
			void onCustomColorsToggled(bool);
			void onPlaySoundToggled(bool);
			void onCustomSoundToggled(bool);
			void onSoundFileButtonClicked();

		private:
			QDataWidgetMapper * mapper_;
			QtHighlightRulesItemModel * model_;
			bool hasValidIndex_;
			Ui::QtHighlightRuleWidget ui_;
	};

}
