/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtCheckBoxStyledItemDelegate.h>

#include <array>

#include <QApplication>
#include <QEvent>
#include <QPainter>

namespace Swift {

QtCheckBoxStyledItemDelegate::QtCheckBoxStyledItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

void QtCheckBoxStyledItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionButton cbOpt;
    cbOpt.rect = option.rect;
    cbOpt.state = QStyle::State_Active;

    auto copyFlags = std::array<QStyle::StateFlag, 2>({{QStyle::State_Enabled/*, QStyle::State_Sunken*/}});
    for (auto flag : copyFlags) {
        if (option.state && flag) {
            cbOpt.state |= flag;
        }
    }

    bool isChecked = index.data(DATA_ROLE).toBool();
    if (isChecked) {
        cbOpt.state |= QStyle::State_On;
    }
    else {
        cbOpt.state |= QStyle::State_Off;
    }
    painter->fillRect(option.rect, option.state & QStyle::State_Selected ? option.palette.highlight() : option.palette.base());
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbOpt, painter);
}

bool QtCheckBoxStyledItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) {
    if (event->type() == QEvent::MouseButtonRelease) {
        model->setData(index, !index.data(DATA_ROLE).toBool(), DATA_ROLE);
    }
    return true;
}

};
