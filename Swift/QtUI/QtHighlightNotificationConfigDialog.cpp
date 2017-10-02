/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtHighlightNotificationConfigDialog.h>

#include <Swiften/Base/Log.h>

#include <Swift/Controllers/Highlighting/HighlightManager.h>

#include <Swift/QtUI/QtCheckBoxStyledItemDelegate.h>
#include <Swift/QtUI/QtColorSelectionStyledItemDelegate.h>
#include <Swift/QtUI/QtSoundSelectionStyledItemDelegate.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtHighlightNotificationConfigDialog::QtHighlightNotificationConfigDialog(QtSettingsProvider* settings, QWidget* parent) : QDialog(parent), settings_(settings) {
    ui_.setupUi(this);

    // setup custom delegates for checkboxes, color selection, and sound selection
    ui_.userHighlightTreeWidget->setItemDelegateForColumn(1, new QtColorSelectionStyledItemDelegate(this));
    ui_.userHighlightTreeWidget->setItemDelegateForColumn(2, new QtColorSelectionStyledItemDelegate(this));
    ui_.userHighlightTreeWidget->setItemDelegateForColumn(3, new QtSoundSelectionStyledItemDelegate(this));
    ui_.userHighlightTreeWidget->setItemDelegateForColumn(4, new QtCheckBoxStyledItemDelegate(this));

    ui_.keywordHighlightTreeWidget->setItemDelegateForColumn(1, new QtCheckBoxStyledItemDelegate(this));
    ui_.keywordHighlightTreeWidget->setItemDelegateForColumn(2, new QtColorSelectionStyledItemDelegate(this));
    ui_.keywordHighlightTreeWidget->setItemDelegateForColumn(3, new QtColorSelectionStyledItemDelegate(this));
    ui_.keywordHighlightTreeWidget->setItemDelegateForColumn(4, new QtSoundSelectionStyledItemDelegate(this));
    ui_.keywordHighlightTreeWidget->setItemDelegateForColumn(5, new QtCheckBoxStyledItemDelegate(this));

    // user highlight edit slots
    connect(ui_.addUserHighlightPushButton, &QPushButton::clicked, [&](bool) {
        auto item = new QTreeWidgetItem();
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
        item->setData(0, Qt::EditRole, "");
        item->setData(1, QtColorSelectionStyledItemDelegate::DATA_ROLE, QColor("#000000"));
        item->setData(2, QtColorSelectionStyledItemDelegate::DATA_ROLE, QColor("#ffff00"));
        item->setData(3, Qt::EditRole, "");
        item->setData(4, QtCheckBoxStyledItemDelegate::DATA_ROLE, QVariant(true));
        ui_.userHighlightTreeWidget->addTopLevelItem(item);
    });
    connect(ui_.removeUserHighlightPushButton, &QPushButton::clicked, [&](bool) {
        auto currentItem = ui_.userHighlightTreeWidget->currentItem();
        if (currentItem) {
            ui_.userHighlightTreeWidget->takeTopLevelItem(ui_.userHighlightTreeWidget->indexOfTopLevelItem(currentItem));
        }
    });
    connect(ui_.userHighlightTreeWidget, &QTreeWidget::currentItemChanged, [&](QTreeWidgetItem* current, QTreeWidgetItem* ) {
        ui_.removeUserHighlightPushButton->setEnabled(current != 0);
    });

    // keyword highlight edit slots
    connect(ui_.addKeywordHighlightPushButton, &QPushButton::clicked, [&](bool) {
        auto item = new QTreeWidgetItem();
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
        item->setData(0, Qt::EditRole, "");
        item->setData(1, QtCheckBoxStyledItemDelegate::DATA_ROLE, QVariant(false));
        item->setData(2, QtColorSelectionStyledItemDelegate::DATA_ROLE, QColor("#000000"));
        item->setData(3, QtColorSelectionStyledItemDelegate::DATA_ROLE, QColor("#ffff00"));
        item->setData(4, Qt::EditRole, "");
        item->setData(5, QtCheckBoxStyledItemDelegate::DATA_ROLE, QVariant(true));
        ui_.keywordHighlightTreeWidget->addTopLevelItem(item);
    });
    connect(ui_.removeKeywordHighlightPushButton, &QPushButton::clicked, [&](bool) {
        auto currentItem = ui_.keywordHighlightTreeWidget->currentItem();
        if (currentItem) {
            ui_.keywordHighlightTreeWidget->takeTopLevelItem(ui_.keywordHighlightTreeWidget->indexOfTopLevelItem(currentItem));
        }
    });
    connect(ui_.keywordHighlightTreeWidget, &QTreeWidget::currentItemChanged, [&](QTreeWidgetItem* current, QTreeWidgetItem* ) {
        ui_.removeKeywordHighlightPushButton->setEnabled(current != 0);
    });

    // setup slots for main dialog buttons
    connect(ui_.buttonBox, &QDialogButtonBox::clicked, [&](QAbstractButton* clickedButton) {
        if (clickedButton == ui_.buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
            if (highlightManager_) {
                highlightManager_->resetToDefaultConfiguration();
                setHighlightConfigurationToDialog(*highlightManager_->getConfiguration());
            }
        }
    });
    connect(this, &QDialog::accepted, [&]() {
        if (highlightManager_) {
            highlightManager_->setConfiguration(getHighlightConfigurationFromDialog());
        }
    });
}

QtHighlightNotificationConfigDialog::~QtHighlightNotificationConfigDialog() {
}

void QtHighlightNotificationConfigDialog::show() {
    if (highlightManager_) {
        setHighlightConfigurationToDialog(*(highlightManager_->getConfiguration()));
    }
    QWidget::show();
    QWidget::activateWindow();
}

void QtHighlightNotificationConfigDialog::setHighlightManager(HighlightManager* highlightManager) {
    highlightManager_ = highlightManager;
}

void QtHighlightNotificationConfigDialog::setContactSuggestions(const std::vector<Contact::ref>& /*suggestions*/) {

}

HighlightConfiguration QtHighlightNotificationConfigDialog::getHighlightConfigurationFromDialog() const {
    auto qtColorToOptionalString = [&](const QColor& color) {
        boost::optional<std::string> colorString;
        if (color.isValid()) {
            colorString = Q2PSTRING(color.name(QColor::HexRgb));
        }
        return colorString;
    };

    auto getHighlightActionFromWidgetItem = [&](const QTreeWidgetItem* item, int startingColumn) {
        HighlightAction action;

        action.setFrontColor(qtColorToOptionalString(item->data(startingColumn, QtColorSelectionStyledItemDelegate::DATA_ROLE).value<QColor>()));
        action.setBackColor(qtColorToOptionalString(item->data(startingColumn + 1, QtColorSelectionStyledItemDelegate::DATA_ROLE).value<QColor>()));

        std::string soundFilePath = Q2PSTRING(item->data(startingColumn + 2, Qt::EditRole).toString());
        if (soundFilePath == "defaultSound") {
            action.setSoundFilePath(boost::optional<std::string>(""));
        }
        else if (soundFilePath.empty()) {
            action.setSoundFilePath(boost::optional<std::string>());
        }
        else {
            action.setSoundFilePath(boost::optional<std::string>(""));
        }

        action.setSystemNotificationEnabled(item->data(startingColumn + 3, QtCheckBoxStyledItemDelegate::DATA_ROLE).toBool());
        return action;
    };

    HighlightConfiguration uiConfiguration;

    // contact highlights
    for (int i = 0; i < ui_.userHighlightTreeWidget->topLevelItemCount(); i++) {
        auto item = ui_.userHighlightTreeWidget->topLevelItem(i);
        HighlightConfiguration::ContactHighlight contactHighlight;
        contactHighlight.name = Q2PSTRING(item->data(0, Qt::EditRole).toString());
        contactHighlight.action = getHighlightActionFromWidgetItem(item, 1);
        uiConfiguration.contactHighlights.push_back(contactHighlight);
    }

    // keyword highlights
    for (int i = 0; i < ui_.keywordHighlightTreeWidget->topLevelItemCount(); i++) {
        auto item = ui_.keywordHighlightTreeWidget->topLevelItem(i);
        HighlightConfiguration::KeywordHightlight keywordHighlight;
        keywordHighlight.keyword = Q2PSTRING(item->data(0, Qt::EditRole).toString());
        keywordHighlight.matchCaseSensitive = item->data(1, QtCheckBoxStyledItemDelegate::DATA_ROLE).toBool();
        keywordHighlight.action = getHighlightActionFromWidgetItem(item, 2);
        uiConfiguration.keywordHighlights.push_back(keywordHighlight);
    }

    // general configuration
    uiConfiguration.playSoundOnIncomingDirectMessages = ui_.playSoundOnDirectMessagesCheckBox->isChecked();
    uiConfiguration.showNotificationOnIncomingDirectMessages = ui_.notificationOnDirectMessagesCheckBox->isChecked();
    uiConfiguration.playSoundOnIncomingGroupchatMessages = ui_.playSoundOnGroupMessagesCheckBox->isChecked();
    uiConfiguration.showNotificationOnIncomingGroupchatMessages = ui_.notificationOnGroupMessagesCheckBox->isChecked();

    uiConfiguration.ownMentionAction.setFrontColor(qtColorToOptionalString(ui_.mentionTextColorColorButton->getColor()));
    uiConfiguration.ownMentionAction.setBackColor(qtColorToOptionalString(ui_.mentionBackgroundColorButton->getColor()));
    uiConfiguration.ownMentionAction.setSoundFilePath(ui_.playSoundOnMentionCheckBox->isChecked() ? boost::optional<std::string>("") : boost::optional<std::string>());
    uiConfiguration.ownMentionAction.setSystemNotificationEnabled(ui_.notificationOnMentionCheckBox->isChecked());
    return uiConfiguration;
}

void QtHighlightNotificationConfigDialog::setHighlightConfigurationToDialog(const HighlightConfiguration& config) {
    auto optionalStringToQtColor = [](const boost::optional<std::string>& colorString) {
        QColor qtColor;
        if (colorString) {
            qtColor = QColor(P2QSTRING(colorString.get_value_or(std::string(""))));
        }
        return qtColor;
    };

    auto optionalSoundPathStringToQString = [](const boost::optional<std::string>& soundPath) {
        QString ret;
        if (soundPath) {
            if (soundPath.get_value_or("").empty()) {
                ret = "defaultSound";
            }
            else {
                ret = P2QSTRING(soundPath.get_value_or(""));
            }
        }
        return ret;
    };

    auto setHighlightActionOnTreeWidgetItem = [&](QTreeWidgetItem* item, int startingColumn, const HighlightAction& action) {
        item->setData(startingColumn, QtColorSelectionStyledItemDelegate::DATA_ROLE, optionalStringToQtColor(action.getFrontColor()));
        item->setData(startingColumn + 1, QtColorSelectionStyledItemDelegate::DATA_ROLE, optionalStringToQtColor(action.getBackColor()));
        item->setData(startingColumn + 2, Qt::DisplayRole, P2QSTRING(action.getSoundFilePath().get_value_or(std::string(""))));
        item->setData(startingColumn + 2, Qt::EditRole, optionalSoundPathStringToQString(action.getSoundFilePath()));
        item->setData(startingColumn + 3, QtCheckBoxStyledItemDelegate::DATA_ROLE, action.isSystemNotificationEnabled());
    };

    // contact highlights
    ui_.userHighlightTreeWidget->clear();
    for (const auto& contactHighlight : config.contactHighlights) {
        auto item = new QTreeWidgetItem();
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
        item->setData(0, Qt::DisplayRole, P2QSTRING(contactHighlight.name));
        item->setData(0, Qt::EditRole, P2QSTRING(contactHighlight.name));

        setHighlightActionOnTreeWidgetItem(item, 1, contactHighlight.action);

        ui_.userHighlightTreeWidget->addTopLevelItem(item);
    }

    // keyword highlights
    ui_.keywordHighlightTreeWidget->clear();
    for (const auto& keywordHighlight : config.keywordHighlights) {
        auto item = new QTreeWidgetItem();
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setData(0, Qt::DisplayRole, P2QSTRING(keywordHighlight.keyword));
        item->setData(0, Qt::EditRole, P2QSTRING(keywordHighlight.keyword));
        item->setData(1, QtCheckBoxStyledItemDelegate::DATA_ROLE, keywordHighlight.matchCaseSensitive);

        setHighlightActionOnTreeWidgetItem(item, 2, keywordHighlight.action);

        ui_.keywordHighlightTreeWidget->addTopLevelItem(item);
    }

    // general configuration
    ui_.playSoundOnDirectMessagesCheckBox->setChecked(config.playSoundOnIncomingDirectMessages);
    ui_.notificationOnDirectMessagesCheckBox->setChecked(config.showNotificationOnIncomingDirectMessages);
    ui_.playSoundOnGroupMessagesCheckBox->setChecked(config.playSoundOnIncomingGroupchatMessages);
    ui_.notificationOnGroupMessagesCheckBox->setChecked(config.showNotificationOnIncomingGroupchatMessages);

    ui_.mentionTextColorColorButton->setColor(optionalStringToQtColor(config.ownMentionAction.getFrontColor()));
    ui_.mentionBackgroundColorButton->setColor(optionalStringToQtColor(config.ownMentionAction.getBackColor()));
    ui_.playSoundOnMentionCheckBox->setChecked(config.ownMentionAction.getSoundFilePath().is_initialized());
    ui_.notificationOnMentionCheckBox->setChecked(config.ownMentionAction.isSystemNotificationEnabled());
}

}
