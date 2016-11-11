/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QDialog>

#include <Swift/QtUI/ui_QtUpdateFeedSelectionDialog.h>

namespace Swift {

class SettingsProvider;

class QtUpdateFeedSelectionDialog : public QDialog {
        Q_OBJECT
    public:
        QtUpdateFeedSelectionDialog(SettingsProvider* settingsProvider);

    private:
        void setDescriptionForIndex(int index);

    private:
        Ui::QtUpdateFeedSelectionDialog ui;
        SettingsProvider* settings_ = nullptr;
};

}
