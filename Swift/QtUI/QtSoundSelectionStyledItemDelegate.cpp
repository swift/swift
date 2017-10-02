/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtSoundSelectionStyledItemDelegate.h>

#include <QApplication>
#include <QComboBox>
#include <QEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionComboBox>

#include <Swiften/Base/Log.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtSoundSelectionStyledItemDelegate::QtSoundSelectionStyledItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

void QtSoundSelectionStyledItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    // draw item selected background
    painter->fillRect(option.rect, option.state & QStyle::State_Selected ? option.palette.highlight() : option.palette.base());

    auto editRoleString = index.data(Qt::EditRole).toString();

    // draw combo box
    QStyleOptionComboBox opt;
    opt.rect = option.rect;
    opt.rect.setHeight(opt.rect.height() + 2);
    opt.state = QStyle::State_Active | QStyle::State_Enabled;
    if (editRoleString.isEmpty()) {
        opt.currentText = tr("No sound");
    }
    else if (editRoleString == "defaultSound") {
        opt.currentText = tr("Default sound");
    }
    else {
        opt.currentText = editRoleString;
    }

    painter->save();
    QFont smallFont;
    smallFont.setPointSize(smallFont.pointSize() - 3);
    painter->setFont(smallFont);

    QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &opt, painter);
    QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &opt, painter);
    painter->restore();
}

bool QtSoundSelectionStyledItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) {
    if (event->type() == QEvent::MouseButtonRelease) {
        auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
        assert(mouseEvent);
        auto editRoleString = index.data(Qt::EditRole).toString();

        auto popUpMenu = new QMenu();

        auto noSound = popUpMenu->addAction(tr("No sound"));
        auto defaultSound = popUpMenu->addAction(tr("Default sound"));
        QAction* customSoundFile = nullptr;
        QAction* selectedAction = nullptr;
        if (editRoleString.isEmpty()) {
            selectedAction = noSound;
        }
        else if (editRoleString == "defaultSound") {
            selectedAction = defaultSound;
        }
        else {
            customSoundFile = popUpMenu->addAction(editRoleString);
            selectedAction = customSoundFile;
        }
        if (selectedAction) {
            selectedAction->setCheckable(true);
            selectedAction->setChecked(true);
        }

        selectedAction = popUpMenu->exec(mouseEvent->globalPos(), selectedAction);

        if (selectedAction == defaultSound) {
            model->setData(index, "defaultSound", Qt::EditRole);
        }
        else if (selectedAction == noSound) {
            model->setData(index, "", Qt::EditRole);
        }

        delete popUpMenu;
    }
    return true;
}

};
