/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtVCardWidget/QtTagComboBox.h>

#include <QAbstractItemView>
#include <QtGui>

namespace Swift {

QtTagComboBox::QtTagComboBox(QWidget* parent) : QComboBox(parent) {
    setEditable(false);
    displayModel = new QStandardItemModel(this);
    displayItem = new QStandardItem();
    displayItem->setText("");
    displayModel->insertRow(0, displayItem);
    editMenu = new QMenu(this);
    this->setModel(displayModel);
    editable = true;
}

QtTagComboBox::~QtTagComboBox() {

}

bool QtTagComboBox::isEditable() const {
    return editable;
}

void QtTagComboBox::setEditable(const bool editable) {
    this->editable = editable;
}

void QtTagComboBox::addTag(const QString &id, const QString &label) {
    QAction* tagAction = new QAction(editMenu);
    tagAction->setText(label);
    tagAction->setCheckable(true);
    tagAction->setData(QString(id));
    editMenu->addAction(tagAction);
}

void QtTagComboBox::setTag(const QString &id, bool value) {
    QList<QAction*> tagActions = editMenu->actions();
    for (auto action : tagActions) {
        if (action->data() == id) {
            action->setChecked(value);
            updateDisplayItem();
            return;
        }
    }
}

bool QtTagComboBox::isTagSet(const QString &id) const {
    QList<QAction*> tagActions = editMenu->actions();
    for (auto action : tagActions) {
        if (action->data() == id) {
            return action->isChecked();
        }
    }
    return false;
}

void QtTagComboBox::showPopup() {

}

void QtTagComboBox::hidePopup() {

}

bool QtTagComboBox::event(QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::KeyRelease) {
        if (!editable) return true;

        QPoint p = mapToGlobal(QPoint(0,0));
        p += QPoint(0, height());
        editMenu->exec(p);
        updateDisplayItem();
        return true;
    }
    return QComboBox::event(event);
}

void QtTagComboBox::updateDisplayItem() {
    QList<QAction*> tagActions = editMenu->actions();
    QString text = "";
    for (auto action : tagActions) {
        if (action->isChecked()) {
            if (text != "") {
                text += ", ";
            }
            text += action->text();
        }
    }
    setItemText(0, text);
}

}
