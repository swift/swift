/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtColorSelectionStyledItemDelegate.h>

#include <QApplication>
#include <QColorDialog>
#include <QEvent>
#include <QPainter>
#include <QStyleOptionComboBox>

namespace Swift {

QtColorSelectionStyledItemDelegate::QtColorSelectionStyledItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

void QtColorSelectionStyledItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    // draw item selected background
    painter->fillRect(option.rect, option.state & QStyle::State_Selected ? option.palette.highlight() : option.palette.base());

    // draw combo box
    QStyleOptionComboBox opt;
    opt.rect = option.rect;
    opt.state = QStyle::State_Active | QStyle::State_Enabled;
    QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &opt, painter);

    // draw currently selected color
    auto contentRect = QApplication::style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField);
    auto currentColor = index.data(DATA_ROLE).value<QColor>();
    painter->save();
    painter->setClipRect(contentRect);
    painter->fillRect(contentRect.adjusted(1, -4, -1, -3), currentColor);
    painter->restore();
}

bool QtColorSelectionStyledItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) {
    if (event->type() == QEvent::MouseButtonRelease) {
        auto currentColor = index.data(DATA_ROLE).value<QColor>();
        auto newColor = QColorDialog::getColor(currentColor);
        if (newColor.isValid()) {
            model->setData(index, newColor, DATA_ROLE);
        }

        auto correspondingDialog = qobject_cast<QDialog*>(parent());
        if (correspondingDialog) {
            correspondingDialog->raise();
        }
    }
    return true;
}

};
