/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/Highlighting/HighlightConfiguration.h>
#include <Swift/Controllers/UIInterfaces/HighlightEditorWindow.h>

#include <Swift/QtUI/ui_QtHighlightNotificationConfigDialog.h>

namespace Swift {

    class QtSettingsProvider;
    class QtSuggestingJIDInput;

    class QtHighlightNotificationConfigDialog : public QDialog, public HighlightEditorWindow {
        Q_OBJECT

        public:
            QtHighlightNotificationConfigDialog(QtSettingsProvider* settings, QWidget* parent = nullptr);
            virtual ~QtHighlightNotificationConfigDialog();

            virtual void show();
            virtual void setHighlightManager(HighlightManager* highlightManager);
            virtual void setContactSuggestions(const std::vector<Contact::ref>& suggestions);

        private:
            HighlightConfiguration getHighlightConfigurationFromDialog() const;
            void setHighlightConfigurationToDialog(const HighlightConfiguration& config);

        private:
            Ui::QtHighlightNotificationConfigDialog ui_;
            QtSettingsProvider* settings_;
            HighlightManager* highlightManager_ = nullptr;
            QtSuggestingJIDInput* jid_ = nullptr;
        };

}
