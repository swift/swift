/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtUpdateFeedSelectionDialog.h>

#include <QComboBox>

#include <Swift/Controllers/Settings/SettingsProvider.h>

#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/SwiftUpdateFeeds.h>

namespace Swift {

QtUpdateFeedSelectionDialog::QtUpdateFeedSelectionDialog(SettingsProvider* settingsProvider) : QDialog(), settings_(settingsProvider) {
    ui.setupUi(this);

    connect(ui.currentChannelComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&] (int newIndex) {
        setDescriptionForIndex(newIndex);
    });

    auto updateChannel = settings_->getSetting(QtUISettingConstants::SOFTWARE_UPDATE_CHANNEL);
    if (updateChannel == UpdateFeeds::StableChannel) {
        ui.currentChannelComboBox->setCurrentIndex(0);
    }
    else if (updateChannel == UpdateFeeds::TestingChannel) {
        ui.currentChannelComboBox->setCurrentIndex(1);
    }
    else if (updateChannel == UpdateFeeds::DevelopmentChannel) {
        ui.currentChannelComboBox->setCurrentIndex(2);
    }

    connect(this, &QDialog::accepted, [&]() {
        auto newUpdateChannel = std::string("");
        switch (ui.currentChannelComboBox->currentIndex()) {
            case 0:
                newUpdateChannel = UpdateFeeds::StableChannel;
                break;
            case 1:
                newUpdateChannel = UpdateFeeds::TestingChannel;
                break;
            case 2:
                newUpdateChannel = UpdateFeeds::DevelopmentChannel;
            break;
        }
        settings_->storeSetting(QtUISettingConstants::SOFTWARE_UPDATE_CHANNEL, newUpdateChannel);
    });

    setAttribute(Qt::WA_DeleteOnClose);
}

void QtUpdateFeedSelectionDialog::setDescriptionForIndex(int index) {
    switch (index) {
        case 0:
            ui.stableDescriptionLabel->show();
            ui.testingDescriptionLabel->hide();
            ui.developmentDescriptionLabel->hide();
            break;
        case 1:
            ui.stableDescriptionLabel->hide();
            ui.testingDescriptionLabel->show();
            ui.developmentDescriptionLabel->hide();
            break;
        case 2:
            ui.stableDescriptionLabel->hide();
            ui.testingDescriptionLabel->hide();
            ui.developmentDescriptionLabel->show();
            break;
        default:
            ui.stableDescriptionLabel->hide();
            ui.testingDescriptionLabel->hide();
            ui.developmentDescriptionLabel->hide();
            break;
    }
    setFixedSize(sizeHint());
}



}
