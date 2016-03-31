/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/HighlightRule.h>
#include <Swift/Controllers/UIInterfaces/HighlightEditorWindow.h>

#include <Swift/QtUI/ui_QtHighlightEditor.h>

namespace Swift {

    class QtSettingsProvider;
    class QtSuggestingJIDInput;
    class QtWebKitChatView;

    class QtHighlightEditor : public QWidget, public HighlightEditorWindow {
        Q_OBJECT

        public:
            QtHighlightEditor(QtSettingsProvider* settings, QWidget* parent = NULL);
            virtual ~QtHighlightEditor();

            virtual void show();
            virtual void setHighlightManager(HighlightManager* highlightManager);
            virtual void setContactSuggestions(const std::vector<Contact::ref>& suggestions);

        private slots:
            void colorOtherSelect();
            void colorCustomSelect();
            void soundOtherSelect();
            void soundCustomSelect();
            void onNewButtonClicked();
            void onDeleteButtonClicked();
            void onUpButtonClicked();
            void onDownButtonClicked();
            void onCurrentRowChanged(int currentRow);
            void onApplyButtonClick();
            void onCancelButtonClick();
            void onOkButtonClick();
            void setChildWidgetStates();
            void widgetClick();
            void disableDialog();
            void handleContactSuggestionRequested(const QString& text);
            void selectSoundFile();
            void onResetToDefaultRulesClicked();

        private:
            void handleOnUserSelected(const Contact::ref& contact);
            void populateList();
            void selectRow(int row);
            int getSelectedRow() const;
            HighlightRule ruleFromDialog();
            void ruleToDialog(const HighlightRule& rule);
            void updateResetToDefaultRulesVisibility();
            void moveRowFromTo(int fromRow, int toRow);

            Ui::QtHighlightEditor ui_;
            QtSettingsProvider* settings_;
            HighlightManager* highlightManager_;
            QtSuggestingJIDInput* jid_;
            int previousRow_;
        };

}
