/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swift/QtUI/QtSpellCheckerWindow.h"

#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/QtSwiftUtil.h>

#include <QCoreApplication>
#include <QFileDialog>
#include <QDir>
#include <QStringList>

namespace Swift {

QtSpellCheckerWindow::QtSpellCheckerWindow(SettingsProvider* settings, QWidget* parent) : QDialog(parent) {
	settings_ = settings;
	ui_.setupUi(this);
	connect(ui_.spellChecker, SIGNAL(toggled(bool)), this, SLOT(handleChecker(bool)));
	connect(ui_.cancel, SIGNAL(clicked()), this, SLOT(handleCancel()));
	connect(ui_.apply, SIGNAL(clicked()), this, SLOT(handleApply()));
	connect(ui_.pathButton, SIGNAL(clicked()), this, SLOT(handlePathButton()));
	setFromSettings();
}

void QtSpellCheckerWindow::setFromSettings() {
	ui_.spellChecker->setChecked(settings_->getSetting(SettingConstants::SPELL_CHECKER));
	ui_.pathContent->setText(P2QSTRING(settings_->getSetting(SettingConstants::DICT_PATH)));
	ui_.currentLanguageValue->setText(P2QSTRING(settings_->getSetting(SettingConstants::DICT_FILE)));
	std::string currentPath = settings_->getSetting(SettingConstants::DICT_PATH);
	QString filename = "*.dic";
	QDir dictDirectory = QDir(P2QSTRING(currentPath));
	QStringList files = dictDirectory.entryList(QStringList(filename), QDir::Files);
	showFiles(files);
	setEnabled(settings_->getSetting(SettingConstants::SPELL_CHECKER));
}

void QtSpellCheckerWindow::handleChecker(bool state) {
	setEnabled(state);
}

void QtSpellCheckerWindow::setEnabled(bool state) {
	ui_.pathContent->setEnabled(state);
	ui_.languageView->setEnabled(state);
	ui_.pathButton->setEnabled(state);
	ui_.pathLabel->setEnabled(state);
	ui_.currentLanguage->setEnabled(state);
	ui_.currentLanguageValue->setEnabled(state);
	ui_.language->setEnabled(state);
}

void QtSpellCheckerWindow::handleApply() {
	settings_->storeSetting(SettingConstants::SPELL_CHECKER, ui_.spellChecker->isChecked());
	QList<QListWidgetItem* > selectedLanguage = ui_.languageView->selectedItems();
	if (!selectedLanguage.empty()) {
		settings_->storeSetting(SettingConstants::DICT_FILE, Q2PSTRING((selectedLanguage.first())->text()));
	}
	this->done(0);
}

void QtSpellCheckerWindow::handleCancel() {
	this->done(0);
}

void QtSpellCheckerWindow::handlePathButton() {
	std::string currentPath = settings_->getSetting(SettingConstants::DICT_PATH);
	QString dirpath = QFileDialog::getExistingDirectory(this, tr("Dictionary Path"), P2QSTRING(currentPath));
	if (dirpath != P2QSTRING(currentPath)) {
		ui_.languageView->clear();
		settings_->storeSetting(SettingConstants::DICT_FILE, "");
		ui_.currentLanguageValue->setText(" ");
	}
	if (!dirpath.isEmpty()) {
		if (!dirpath.endsWith("/")) {
			dirpath.append("/");
		}
		settings_->storeSetting(SettingConstants::DICT_PATH, Q2PSTRING(dirpath));
		QDir dictDirectory = QDir(dirpath);
		ui_.pathContent->setText(dirpath);
		QString filename = "*.dic";
		QStringList files = dictDirectory.entryList(QStringList(filename), QDir::Files);
		showFiles(files);
	}
}

void QtSpellCheckerWindow::handlePersonalPathButton() {
	std::string currentPath = settings_->getSetting(SettingConstants::PERSONAL_DICT_PATH);
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Personal Dictionary"), P2QSTRING(currentPath), tr("(*.dic"));
	settings_->storeSetting(SettingConstants::PERSONAL_DICT_PATH, Q2PSTRING(filename));
}

void QtSpellCheckerWindow::showFiles(const QStringList& files) {
	ui_.languageView->clear();
	for (int i = 0; i < files.size(); ++i) {
		ui_.languageView->insertItem(i, files[i]);
	}
}

}
