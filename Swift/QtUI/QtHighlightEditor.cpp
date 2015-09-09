/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtHighlightEditor.h>

#include <cassert>

#include <boost/lexical_cast.hpp>

#include <QFileDialog>
#include <QTreeWidgetItem>

#include <Swift/Controllers/HighlightManager.cpp>

#include <Swift/QtUI/QtSettingsProvider.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/UserSearch/QtSuggestingJIDInput.h>

namespace Swift {

QtHighlightEditor::QtHighlightEditor(QtSettingsProvider* settings, QWidget* parent)
	: QWidget(parent), settings_(settings), previousRow_(-1)
{
	ui_.setupUi(this);

	connect(ui_.listWidget, SIGNAL(currentRowChanged(int)), SLOT(onCurrentRowChanged(int)));

	connect(ui_.newButton, SIGNAL(clicked()), SLOT(onNewButtonClicked()));
	connect(ui_.deleteButton, SIGNAL(clicked()), SLOT(onDeleteButtonClicked()));

	connect(ui_.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), SLOT(onApplyButtonClick()));
	connect(ui_.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(onCancelButtonClick()));
	connect(ui_.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(onOkButtonClick()));
	connect(ui_.buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), SLOT(onResetToDefaultRulesClicked()));

	connect(ui_.noColorRadio, SIGNAL(clicked()), SLOT(colorOtherSelect()));
	connect(ui_.customColorRadio, SIGNAL(clicked()), SLOT(colorCustomSelect()));

	connect(ui_.noSoundRadio, SIGNAL(clicked()), SLOT(soundOtherSelect()));
	connect(ui_.defaultSoundRadio, SIGNAL(clicked()), SLOT(soundOtherSelect()));
	connect(ui_.customSoundRadio, SIGNAL(clicked()), SLOT(soundCustomSelect()));

	/* replace the static line-edit control with the roster autocompleter */
	ui_.dummySenderName->setVisible(false);
	jid_ = new QtSuggestingJIDInput(this, settings);
	ui_.senderName->addWidget(jid_);
	jid_->onUserSelected.connect(boost::bind(&QtHighlightEditor::handleOnUserSelected, this, _1));

	/* handle autocomplete */
	connect(jid_, SIGNAL(textEdited(QString)), SLOT(handleContactSuggestionRequested(QString)));

	/* we need to be notified if any of the state changes so that we can update our textual rule description */
	connect(ui_.chatRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.roomRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.nickIsKeyword, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.allMsgRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.senderRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(jid_, SIGNAL(textChanged(const QString&)), SLOT(widgetClick()));
	connect(ui_.keywordRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.keyword, SIGNAL(textChanged(const QString&)), SLOT(widgetClick()));
	connect(ui_.matchPartialWords, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.matchCase, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.noColorRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.customColorRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.noSoundRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.defaultSoundRadio, SIGNAL(clicked()), SLOT(widgetClick()));
	connect(ui_.customSoundRadio, SIGNAL(clicked()), SLOT(widgetClick()));

	/* allow selection of a custom sound file */
	connect(ui_.soundFileButton, SIGNAL(clicked()), SLOT(selectSoundFile()));

	/* allowing reordering items */
	connect(ui_.moveUpButton, SIGNAL(clicked()), this, SLOT(onUpButtonClicked()));
	connect(ui_.moveDownButton, SIGNAL(clicked()), this, SLOT(onDownButtonClicked()));

	setWindowTitle(tr("Highlight Rules"));
}

QtHighlightEditor::~QtHighlightEditor()
{
}

std::string formatShortDescription(const HighlightRule &rule)
{
	const std::string chatOrRoom = (rule.getMatchChat() ? "chat" : "room");

	std::vector<std::string> senders = rule.getSenders();
	std::vector<std::string> keywords = rule.getKeywords();

	if (senders.empty() && keywords.empty() && !rule.getNickIsKeyword()) {
		return std::string("All ") + chatOrRoom + " messages.";
	}

	if (rule.getNickIsKeyword()) {
		return std::string("All ") + chatOrRoom + " messages that mention my name.";
	}

	if (!senders.empty()) {
		return std::string("All ") + chatOrRoom + " messages from " + senders[0] + ".";
	}

	if (!keywords.empty()) {
		return std::string("All ") + chatOrRoom + " messages mentioning the keyword '" + keywords[0] + "'.";
	}

	return "Unknown Rule";
}

void QtHighlightEditor::show()
{
	highlightManager_->loadSettings();

	populateList();

	if (ui_.listWidget->count()) {
		selectRow(0);
	}

	updateResetToDefaultRulesVisibility();

	/* prepare default states */
	widgetClick();

	QWidget::show();
	QWidget::activateWindow();
}

void QtHighlightEditor::setHighlightManager(HighlightManager* highlightManager)
{
	highlightManager_ = highlightManager;
}

void QtHighlightEditor::setContactSuggestions(const std::vector<Contact::ref>& suggestions)
{
	jid_->setSuggestions(suggestions);
}

void QtHighlightEditor::colorOtherSelect()
{
	ui_.foregroundColor->setEnabled(false);
	ui_.backgroundColor->setEnabled(false);
}

void QtHighlightEditor::colorCustomSelect()
{
	ui_.foregroundColor->setEnabled(true);
	ui_.backgroundColor->setEnabled(true);
}

void QtHighlightEditor::soundOtherSelect()
{
	ui_.soundFile->setEnabled(false);
	ui_.soundFileButton->setEnabled(false);
}

void QtHighlightEditor::soundCustomSelect()
{
	ui_.soundFile->setEnabled(true);
	ui_.soundFileButton->setEnabled(true);
}

void QtHighlightEditor::onNewButtonClicked()
{
	int row = getSelectedRow() + 1;
	populateList();
	HighlightRule newRule;
	newRule.setMatchMUC(true);
	highlightManager_->insertRule(row, newRule);
	QListWidgetItem *item = new QListWidgetItem();
	item->setText(P2QSTRING(formatShortDescription(newRule)));
	QFont font;
	font.setItalic(true);
	item->setFont(font);
	ui_.listWidget->insertItem(row, item);
	selectRow(row);
}

void QtHighlightEditor::onDeleteButtonClicked()
{
	int selectedRow = getSelectedRow();
	assert(selectedRow>=0 && selectedRow<ui_.listWidget->count());
	delete ui_.listWidget->takeItem(selectedRow);
	highlightManager_->removeRule(selectedRow);

	if (!ui_.listWidget->count()) {
		disableDialog();
		ui_.deleteButton->setEnabled(false);
	} else {
		if (selectedRow == ui_.listWidget->count()) {
			selectRow(ui_.listWidget->count() - 1);
		} else {
			selectRow(selectedRow);
		}
	}
}

void QtHighlightEditor::onUpButtonClicked() {
	const size_t moveFrom = ui_.listWidget->currentRow();
	const size_t moveTo = moveFrom - 1;
	highlightManager_->swapRules(moveFrom, moveTo);
	populateList();
	selectRow(moveTo);
}

void QtHighlightEditor::onDownButtonClicked() {
	const size_t moveFrom = ui_.listWidget->currentRow();
	const size_t moveTo = moveFrom + 1;
	highlightManager_->swapRules(moveFrom, moveTo);
	populateList();
	selectRow(moveTo);
}

void QtHighlightEditor::onCurrentRowChanged(int currentRow)
{
	ui_.deleteButton->setEnabled(currentRow != -1);
	ui_.moveUpButton->setEnabled(currentRow != -1 && currentRow != 0);
	ui_.moveDownButton->setEnabled(currentRow != -1 && currentRow != (ui_.listWidget->count()-1));

	if (previousRow_ != -1) {
		if (ui_.listWidget->count() > previousRow_) {
			QFont font;
			font.setItalic(false);
			ui_.listWidget->item(previousRow_)->setFont(font);
			highlightManager_->setRule(previousRow_, ruleFromDialog());
		}
	}

	if (currentRow != -1) {
		HighlightRule rule = highlightManager_->getRule(currentRow);
		ruleToDialog(rule);
		if (ui_.listWidget->currentItem()) {
			QFont font;
			font.setItalic(true);
			ui_.listWidget->currentItem()->setFont(font);
			ui_.listWidget->currentItem()->setText(P2QSTRING(formatShortDescription(rule)));
		}
	}

	/* grey the dialog if we have nothing selected */
	if (currentRow == -1) {
		disableDialog();
	}

	previousRow_ = currentRow;

	updateResetToDefaultRulesVisibility();
}

void QtHighlightEditor::onApplyButtonClick()
{
	selectRow(getSelectedRow()); /* force save */
	highlightManager_->storeSettings();
}

void QtHighlightEditor::onCancelButtonClick()
{
	close();
}

void QtHighlightEditor::onOkButtonClick()
{
	onApplyButtonClick();
	close();
}

void QtHighlightEditor::setChildWidgetStates()
{
	/* disable appropriate radio button child widgets */

	if (ui_.chatRadio->isChecked()) {
		if (ui_.nickIsKeyword->isChecked()) {
			/* switch to another choice before we disable this button */
			ui_.allMsgRadio->setChecked(true);
		}
		ui_.nickIsKeyword->setEnabled(false);
	} else if (ui_.roomRadio->isChecked()) {
		ui_.nickIsKeyword->setEnabled(true);
	} else { /* chats and rooms */
		ui_.nickIsKeyword->setEnabled(true);
	}

	if (ui_.senderRadio->isChecked()) {
		jid_->setEnabled(true);
	} else {
		jid_->setEnabled(false);
	}

	if (ui_.keywordRadio->isChecked()) {
		ui_.keyword->setEnabled(true);
		ui_.matchPartialWords->setEnabled(true);
		ui_.matchCase->setEnabled(true);
	} else {
		ui_.keyword->setEnabled(false);
		ui_.matchPartialWords->setEnabled(false);
		ui_.matchCase->setEnabled(false);
	}

	if (ui_.chatRadio->isChecked()) {
		ui_.allMsgRadio->setText(tr("Apply to all chat messages"));
	} else {
		ui_.allMsgRadio->setText(tr("Apply to all room messages"));
	}
}

void QtHighlightEditor::widgetClick()
{
	setChildWidgetStates();

	HighlightRule rule = ruleFromDialog();

	if (ui_.listWidget->currentItem()) {
		ui_.listWidget->currentItem()->setText(P2QSTRING(formatShortDescription(rule)));
	}
}

void QtHighlightEditor::disableDialog()
{
	ui_.chatRadio->setEnabled(false);
	ui_.roomRadio->setEnabled(false);
	ui_.allMsgRadio->setEnabled(false);
	ui_.nickIsKeyword->setEnabled(false);
	ui_.senderRadio->setEnabled(false);
	ui_.dummySenderName->setEnabled(false);
	ui_.keywordRadio->setEnabled(false);
	ui_.keyword->setEnabled(false);
	ui_.matchPartialWords->setEnabled(false);
	ui_.matchCase->setEnabled(false);
	ui_.noColorRadio->setEnabled(false);
	ui_.customColorRadio->setEnabled(false);
	ui_.foregroundColor->setEnabled(false);
	ui_.backgroundColor->setEnabled(false);
	ui_.noSoundRadio->setEnabled(false);
	ui_.defaultSoundRadio->setEnabled(false);
	ui_.customSoundRadio->setEnabled(false);
	ui_.soundFile->setEnabled(false);
	ui_.soundFileButton->setEnabled(false);
}

void QtHighlightEditor::handleContactSuggestionRequested(const QString& text)
{
	std::string stdText = Q2PSTRING(text);
	onContactSuggestionsRequested(stdText);
}

void QtHighlightEditor::selectSoundFile()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Select sound file..."), QString(), "Sounds (*.wav)");
	ui_.soundFile->setText(path);
}

void QtHighlightEditor::onResetToDefaultRulesClicked() {
	highlightManager_->resetToDefaultRulesList();
	populateList();
	updateResetToDefaultRulesVisibility();
}

void QtHighlightEditor::handleOnUserSelected(const Contact::ref& contact) {
	/* this might seem like it should be standard behaviour for the suggesting input box, but is not desirable in all cases */
	if (contact->jid.isValid()) {
		jid_->setText(P2QSTRING(contact->jid.toString()));
	} else {
		jid_->setText(P2QSTRING(contact->name));
	}
}

void QtHighlightEditor::populateList()
{
	previousRow_ = -1;
	ui_.listWidget->clear();
	HighlightRulesListPtr rules = highlightManager_->getRules();
	for (size_t i = 0; i < rules->getSize(); ++i) {
		const HighlightRule& rule = rules->getRule(i);
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(P2QSTRING(formatShortDescription(rule)));
		ui_.listWidget->addItem(item);
	}
}

void QtHighlightEditor::selectRow(int row)
{
	for (int i = 0; i < ui_.listWidget->count(); ++i) {
		if (i == row) {
			ui_.listWidget->item(i)->setSelected(true);
			onCurrentRowChanged(i);
		} else {
			ui_.listWidget->item(i)->setSelected(false);
		}
	}
	ui_.listWidget->setCurrentRow(row);
}

int QtHighlightEditor::getSelectedRow() const
{
	for (int i = 0; i < ui_.listWidget->count(); ++i) {
		if (ui_.listWidget->item(i)->isSelected()) {
			return i;
		}
	}
	return -1;
}

HighlightRule QtHighlightEditor::ruleFromDialog()
{
	HighlightRule rule;
	HighlightAction& action = rule.getAction();

	if (ui_.chatRadio->isChecked()) {
		rule.setMatchChat(true);
		rule.setMatchMUC(false);
	} else {
		rule.setMatchChat(false);
		rule.setMatchMUC(true);
	}

	if (ui_.allMsgRadio->isChecked()) {
		action.setHighlightAllText(true);
	}

	if (ui_.senderRadio->isChecked()) {
		QString senderName = jid_->text();
		if (!senderName.isEmpty()) {
			std::vector<std::string> senders;
			senders.push_back(Q2PSTRING(senderName));
			rule.setSenders(senders);
			action.setHighlightAllText(true);
		}
	}

	if (ui_.keywordRadio->isChecked()) {
		QString keywordString = ui_.keyword->text();
		if (!keywordString.isEmpty()) {
			std::vector<std::string> keywords;
			keywords.push_back(Q2PSTRING(keywordString));
			rule.setKeywords(keywords);
		}
	}

	if (ui_.nickIsKeyword->isChecked()) {
		rule.setNickIsKeyword(true);
		rule.setMatchWholeWords(true);
		rule.setMatchCase(true);
	} else {
		rule.setMatchWholeWords(!ui_.matchPartialWords->isChecked());
		rule.setMatchCase(ui_.matchCase->isChecked());
	}

	if (ui_.noColorRadio->isChecked()) {
		action.setTextColor("");
		action.setTextBackground("");
	} else {
		action.setTextColor(Q2PSTRING(ui_.foregroundColor->getColor().name()));
		action.setTextBackground(Q2PSTRING(ui_.backgroundColor->getColor().name()));
	}

	if (ui_.noSoundRadio->isChecked()) {
		action.setPlaySound(false);
	} else if (ui_.defaultSoundRadio->isChecked()) {
		action.setPlaySound(true);
		action.setSoundFile("");
	} else {
		action.setPlaySound(true);
		action.setSoundFile(Q2PSTRING(ui_.soundFile->text()));
	}

	return rule;
}

void QtHighlightEditor::ruleToDialog(const HighlightRule& rule)
{
	ui_.chatRadio->setEnabled(true);
	ui_.roomRadio->setEnabled(true);

	if (rule.getMatchMUC()) {
		ui_.chatRadio->setChecked(false);
		ui_.roomRadio->setChecked(true);
	} else {
		ui_.chatRadio->setChecked(true);
		ui_.roomRadio->setChecked(false);
	}

	ui_.allMsgRadio->setEnabled(true);
	ui_.allMsgRadio->setChecked(true); /* this is the default radio button */
	jid_->setText("");
	ui_.keyword->setText("");
	ui_.matchPartialWords->setChecked(false);
	ui_.matchCase->setChecked(false);

	ui_.nickIsKeyword->setEnabled(true);
	if (rule.getNickIsKeyword()) {
		ui_.nickIsKeyword->setChecked(true);
	}

	ui_.senderRadio->setEnabled(true);
	std::vector<std::string> senders = rule.getSenders();
	if (!senders.empty()) {
		ui_.senderRadio->setChecked(true);
		jid_->setText(P2QSTRING(senders[0]));
	}

	ui_.keywordRadio->setEnabled(true);
	std::vector<std::string> keywords = rule.getKeywords();
	if (!keywords.empty()) {
		ui_.keywordRadio->setChecked(true);
		ui_.keyword->setText(P2QSTRING(keywords[0]));
		ui_.matchPartialWords->setChecked(!rule.getMatchWholeWords());
		ui_.matchCase->setChecked(rule.getMatchCase());
	}

	const HighlightAction& action = rule.getAction();

	ui_.noColorRadio->setEnabled(true);
	ui_.customColorRadio->setEnabled(true);
	if (action.getTextColor().empty() && action.getTextBackground().empty()) {
		ui_.noColorRadio->setChecked(true);
		ui_.foregroundColor->setEnabled(false);
		ui_.backgroundColor->setEnabled(false);
	} else {
		ui_.foregroundColor->setEnabled(true);
		ui_.backgroundColor->setEnabled(true);
		QColor foregroundColor(P2QSTRING(action.getTextColor()));
		ui_.foregroundColor->setColor(foregroundColor);
		QColor backgroundColor(P2QSTRING(action.getTextBackground()));
		ui_.backgroundColor->setColor(backgroundColor);
		ui_.customColorRadio->setChecked(true);
	}

	ui_.noSoundRadio->setEnabled(true);
	ui_.defaultSoundRadio->setEnabled(true);
	ui_.customSoundRadio->setEnabled(true);
	ui_.soundFile->setText("");
	ui_.soundFile->setEnabled(false);
	ui_.soundFileButton->setEnabled(false);
	if (action.playSound()) {
		if (action.getSoundFile().empty()) {
			ui_.defaultSoundRadio->setChecked(true);
		} else {
			ui_.customSoundRadio->setChecked(true);
			ui_.soundFile->setText(P2QSTRING(action.getSoundFile()));
			ui_.soundFile->setEnabled(true);
			ui_.soundFileButton->setEnabled(true);
		}
	} else {
		ui_.noSoundRadio->setChecked(true);
	}

	/* set radio button child option states */
	setChildWidgetStates();
}

void QtHighlightEditor::updateResetToDefaultRulesVisibility() {
	ui_.buttonBox->button(QDialogButtonBox::RestoreDefaults)->setVisible(!highlightManager_->isDefaultRulesList());
}

}
