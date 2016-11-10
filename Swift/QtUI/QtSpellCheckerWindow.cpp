/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtSpellCheckerWindow.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QStringList>
#include <QTimer>

#include <Swiften/Base/Log.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUISettingConstants.h>

namespace Swift {

QtSpellCheckerWindow::QtSpellCheckerWindow(SettingsProvider* settings, QWidget* parent) : QDialog(parent) {
    settings_ = settings;
    ui_.setupUi(this);
#ifdef HAVE_HUNSPELL
    ui_.hunspellOptions->show();
#else
    ui_.hunspellOptions->hide();
    QTimer::singleShot(0, this, SLOT(shrinkWindow()));
#endif
    connect(ui_.spellChecker, SIGNAL(toggled(bool)), this, SLOT(handleChecker(bool)));
    connect(ui_.cancel, SIGNAL(clicked()), this, SLOT(handleCancel()));
    connect(ui_.apply, SIGNAL(clicked()), this, SLOT(handleApply()));
    setFromSettings();
}

void QtSpellCheckerWindow::shrinkWindow() {
    resize(0,0);
}

void QtSpellCheckerWindow::setFromSettings() {
    ui_.spellChecker->setChecked(settings_->getSetting(QtUISettingConstants::SPELL_CHECKER));
    setEnabled(settings_->getSetting(QtUISettingConstants::SPELL_CHECKER));
}

void QtSpellCheckerWindow::setSupportedLanguages(const std::vector<std::string>& languages) {
    languageItems_.clear();
    ui_.languageView->clear();
    for (const auto& shortLang : languages) {
        auto locale = QLocale(P2QSTRING(shortLang));
        auto label = QString("%1 ( %2 )").arg(locale.nativeLanguageName(), locale.nativeCountryName());

        QListWidgetItem* item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, P2QSTRING(shortLang));
        languageItems_[shortLang] = item;
        ui_.languageView->addItem(item);
    }
}

void QtSpellCheckerWindow::setActiveLanguage(const std::string& language) {
    SWIFT_LOG_ASSERT(languageItems_.find(language) != languageItems_.end(), warning) << "Language '" << language << "' is not available." << std::endl;
    if (languageItems_.find(language) != languageItems_.end()) {
        languageItems_[language]->setSelected(true);
    }
}

void QtSpellCheckerWindow::setAutomaticallyIdentifiesLanguage(bool isAutomaticallyIdentifying) {
    ui_.languageView->setHidden(isAutomaticallyIdentifying);
}

void QtSpellCheckerWindow::handleChecker(bool state) {
    setEnabled(state);
}

void QtSpellCheckerWindow::setEnabled(bool state) {
    ui_.language->setEnabled(state);
}

void QtSpellCheckerWindow::handleApply() {
    settings_->storeSetting(QtUISettingConstants::SPELL_CHECKER, ui_.spellChecker->isChecked());
    QList<QListWidgetItem* > selectedLanguage = ui_.languageView->selectedItems();
    if (!selectedLanguage.empty()) {
        settings_->storeSetting(QtUISettingConstants::SPELL_CHECKER_LANGUAGE, Q2PSTRING(selectedLanguage.first()->data(Qt::UserRole).toString()));
    }
    this->done(0);
}

void QtSpellCheckerWindow::handleCancel() {
    this->done(0);
}

}
