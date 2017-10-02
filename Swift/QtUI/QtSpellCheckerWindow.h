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

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <QDialog>

#include <Swift/QtUI/ui_QtSpellCheckerWindow.h>

class QListWidgetItem;

namespace Swift {
    class SettingsProvider;
    class QtSpellCheckerWindow : public QDialog, protected Ui::QtSpellCheckerWindow {
        Q_OBJECT
        public:
            QtSpellCheckerWindow(SettingsProvider* settings, QWidget* parent = nullptr);

            void setSupportedLanguages(const std::vector<std::string>& languages);
            void setActiveLanguage(const std::string& language);
            void setAutomaticallyIdentifiesLanguage(bool isAutomaticallyIdentifying);

        public slots:
            void handleChecker(bool state);
            void handleCancel();
            void handleApply();

        private slots:
            void shrinkWindow();

        private:
            void setEnabled(bool state);
            void setFromSettings();

            SettingsProvider* settings_;
            Ui::QtSpellCheckerWindow ui_;
            std::unordered_map<std::string, QListWidgetItem*> languageItems_;
    };
}
