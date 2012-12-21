/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <QDataWidgetMapper>
#include <QStringListModel>
#include <QFileDialog>

#include <Swift/QtUI/QtHighlightRuleWidget.h>
#include <Swift/QtUI/QtHighlightRulesItemModel.h>

namespace Swift {

QtHighlightRuleWidget::QtHighlightRuleWidget(QWidget* parent)
	: QWidget(parent)
{
	ui_.setupUi(this);

	QStringList applyToItems;
	for (int i = 0; i < QtHighlightRulesItemModel::ApplyToEOL; ++i) {
		applyToItems << QtHighlightRulesItemModel::getApplyToString(i);
	}
	QStringListModel * applyToModel = new QStringListModel(applyToItems, this);
	ui_.applyTo->setModel(applyToModel);

	connect(ui_.highlightText, SIGNAL(toggled(bool)), SLOT(onHighlightTextToggled(bool)));
	connect(ui_.customColors, SIGNAL(toggled(bool)), SLOT(onCustomColorsToggled(bool)));
	connect(ui_.playSound, SIGNAL(toggled(bool)), SLOT(onPlaySoundToggled(bool)));
	connect(ui_.customSound, SIGNAL(toggled(bool)), SLOT(onCustomSoundToggled(bool)));
	connect(ui_.soundFileButton, SIGNAL(clicked()), SLOT(onSoundFileButtonClicked()));

	mapper_ = new QDataWidgetMapper(this);
	hasValidIndex_ = false;
	model_ = NULL;
}

QtHighlightRuleWidget::~QtHighlightRuleWidget()
{
}

/** Widget does not gain ownership over the model */
void QtHighlightRuleWidget::setModel(QtHighlightRulesItemModel* model)
{
	model_ = model;
	mapper_->setModel(model_);
}

void QtHighlightRuleWidget::setActiveIndex(const QModelIndex& index)
{
	if (index.isValid()) {
		if (!hasValidIndex_) {
			mapper_->addMapping(ui_.applyTo, QtHighlightRulesItemModel::ApplyTo, "currentIndex");
			mapper_->addMapping(ui_.senders, QtHighlightRulesItemModel::Sender, "plainText");
			mapper_->addMapping(ui_.keywords, QtHighlightRulesItemModel::Keyword, "plainText");
			mapper_->addMapping(ui_.nickIsKeyword, QtHighlightRulesItemModel::NickIsKeyword);
			mapper_->addMapping(ui_.matchCase, QtHighlightRulesItemModel::MatchCase);
			mapper_->addMapping(ui_.matchWholeWords, QtHighlightRulesItemModel::MatchWholeWords);
			mapper_->addMapping(ui_.highlightText, QtHighlightRulesItemModel::HighlightText);
			mapper_->addMapping(ui_.foreground, QtHighlightRulesItemModel::TextColor, "color");
			mapper_->addMapping(ui_.background, QtHighlightRulesItemModel::TextBackground, "color");
			mapper_->addMapping(ui_.playSound, QtHighlightRulesItemModel::PlaySound);
			mapper_->addMapping(ui_.soundFile, QtHighlightRulesItemModel::SoundFile);
		}
		mapper_->setCurrentModelIndex(index);
		ui_.customColors->setChecked(ui_.foreground->getColor().isValid() || ui_.background->getColor().isValid());
		ui_.customSound->setChecked(!ui_.soundFile->text().isEmpty());
		ui_.applyTo->focusWidget();
	} else {
		if (hasValidIndex_) {
			mapper_->clearMapping();
		}
	}

	hasValidIndex_ = index.isValid();
}

void QtHighlightRuleWidget::onCustomColorsToggled(bool enabled)
{
	if (!enabled) {
		ui_.foreground->setColor(QColor());
		ui_.background->setColor(QColor());
	}
	ui_.foreground->setEnabled(enabled);
	ui_.background->setEnabled(enabled);
}

void QtHighlightRuleWidget::onCustomSoundToggled(bool enabled)
{
	if (enabled) {
		if (ui_.soundFile->text().isEmpty()) {
			onSoundFileButtonClicked();
		}
	} else {
		ui_.soundFile->clear();
	}
	ui_.soundFile->setEnabled(enabled);
	ui_.soundFileButton->setEnabled(enabled);
}

void QtHighlightRuleWidget::onSoundFileButtonClicked()
{
	QString s = QFileDialog::getOpenFileName(this, tr("Choose sound file"), QString(), tr("Sound files (*.wav)"));
	if (!s.isEmpty()) {
		ui_.soundFile->setText(s);
	}
}

void QtHighlightRuleWidget::onHighlightTextToggled(bool enabled)
{
	ui_.customColors->setEnabled(enabled);
}

void QtHighlightRuleWidget::onPlaySoundToggled(bool enabled)
{
	ui_.customSound->setEnabled(enabled);
}

void QtHighlightRuleWidget::save()
{
	if (hasValidIndex_) {
		mapper_->submit();
	}
}

void QtHighlightRuleWidget::revert()
{
	if (hasValidIndex_) {
		mapper_->revert();
	}
}

}
