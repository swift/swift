/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtBlockListEditorWindow.h>

#include <boost/bind.hpp>

#include <QClipboard>
#include <QLineEdit>
#include <QMovie>
#include <QShortcut>
#include <QStyledItemDelegate>
#include <QValidator>

#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/JID/JID.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>
#include <Swift/QtUI/ui_QtBlockListEditorWindow.h>

namespace Swift {

class QtJIDValidator : public QValidator {
    public:
        QtJIDValidator(QObject* parent) : QValidator(parent) {}
        virtual ~QtJIDValidator() {}
        virtual QValidator::State validate(QString& input, int&) const {
            return JID(Q2PSTRING(input)).isValid() ? Acceptable : Intermediate;
        }
};

class QtJIDValidatedItemDelegate : public QItemDelegate {
    public:
        QtJIDValidatedItemDelegate(QObject* parent) : QItemDelegate(parent) {}
        virtual ~QtJIDValidatedItemDelegate() {}

        virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const {
            QLineEdit *editor = new QLineEdit(parent);
            editor->setValidator(new QtJIDValidator(editor));
            return editor;
        }

        void setEditorData(QWidget *editor, const QModelIndex &index) const {
            QString value = index.model()->data(index, Qt::EditRole).toString();

            QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
            lineEdit->setText(value);
        }

        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
            QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
            QString currentValue = lineEdit->text();
            int pos = 0;
            if (lineEdit->validator()->validate(currentValue, pos) == QValidator::Acceptable) {
                model->setData(index, lineEdit->text(), Qt::EditRole);
            } else {
                model->setData(index, QString(), Qt::EditRole);
            }
        }

        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {
            editor->setGeometry(option.rect);
        }
};

QtBlockListEditorWindow::QtBlockListEditorWindow() : QWidget(), ui(new Ui::QtBlockListEditorWindow), removeItemDelegate(nullptr), editItemDelegate(nullptr) {
    ui->setupUi(this);

    freshBlockListTemplate = tr("Double-click to add contact");

    new QShortcut(QKeySequence::Close, this, SLOT(close()));
    ui->throbberLabel->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), this));

    removeItemDelegate = new QtRemovableItemDelegate(style());
    editItemDelegate = new QtJIDValidatedItemDelegate(this);

    connect(ui->savePushButton, SIGNAL(clicked()), SLOT(applyChanges()));

    ui->blockListTreeWidget->setColumnCount(2);
    ui->blockListTreeWidget->header()->setStretchLastSection(false);
    ui->blockListTreeWidget->header()->resizeSection(1, removeItemDelegate->sizeHint(QStyleOptionViewItem(), QModelIndex()).width());

#if QT_VERSION >= 0x050000
    ui->blockListTreeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
    ui->blockListTreeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
#endif

    ui->blockListTreeWidget->setHeaderHidden(true);
    ui->blockListTreeWidget->setRootIsDecorated(false);
    ui->blockListTreeWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
    ui->blockListTreeWidget->setItemDelegateForColumn(0, editItemDelegate);
    ui->blockListTreeWidget->setItemDelegateForColumn(1, removeItemDelegate);
    connect(ui->blockListTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(handleItemChanged(QTreeWidgetItem*,int)));
    ui->blockListTreeWidget->installEventFilter(this);

    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(freshBlockListTemplate) << "x");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->blockListTreeWidget->addTopLevelItem(item);

    // Allow pasting a newline seperated list of JIDs into the dialog.
    auto pasteShortcut = new QShortcut(QKeySequence::Paste, this);
    connect(pasteShortcut, &QShortcut::activated, [&](){
        auto currentBlocklist = getCurrentBlockList();

        auto clipboardText = QGuiApplication::clipboard()->text();
        auto stringList = clipboardText.split("\n");
        for (const auto& string : stringList) {
            auto jid = JID(Q2PSTRING(string.trimmed()));
            if (jid.isValid()) {
                if (std::find(currentBlocklist.begin(), currentBlocklist.end(), jid) == currentBlocklist.end()) {
                    currentBlocklist.push_back(jid);
                }
            }
        }
        setCurrentBlockList(currentBlocklist);
    });

}

QtBlockListEditorWindow::~QtBlockListEditorWindow() {
}

void QtBlockListEditorWindow::show() {
    QWidget::showNormal();
    QWidget::activateWindow();
    QWidget::raise();
}

void QtBlockListEditorWindow::hide() {
    QWidget::hide();
}

void QtBlockListEditorWindow::handleItemChanged(QTreeWidgetItem *item, int) {
    // check whether changed item contains a valid JID and make it removable
    if (item && item->text(0) != freshBlockListTemplate) {
        item->setText(1, "");
    }

    // check for empty rows and add an empty one so the user can add items
    bool hasEmptyRow = false;
    for( int i = 0; i < ui->blockListTreeWidget->topLevelItemCount(); ++i ) {
        QTreeWidgetItem* row = ui->blockListTreeWidget->topLevelItem(i);
        if (row->text(0) == freshBlockListTemplate) {
            hasEmptyRow = true;
        }
        else if (row->text(0).isEmpty()) {
            ui->blockListTreeWidget->removeItemWidget(row, 0);
        }
    }

    if (!hasEmptyRow) {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(freshBlockListTemplate) << "x");
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->blockListTreeWidget->addTopLevelItem(item);
    }

    if (!item) {
        ui->blockListTreeWidget->setCurrentItem(ui->blockListTreeWidget->topLevelItem(0));
    }
}

void QtBlockListEditorWindow::applyChanges() {
    onSetNewBlockList(getCurrentBlockList());
}

void QtBlockListEditorWindow::setCurrentBlockList(const std::vector<JID> &blockedJIDs) {
    ui->blockListTreeWidget->clear();

    QStringList blockedStrings;
    for (const auto& jid : blockedJIDs) {
        blockedStrings << P2QSTRING(jid.toString());
    }
    blockedStrings.sort();

    for (const auto& jid : blockedStrings) {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(jid) << "");
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->blockListTreeWidget->addTopLevelItem(item);
    }
    handleItemChanged(nullptr,0);
}

void QtBlockListEditorWindow::setBusy(bool isBusy) {
    if (isBusy) {
        ui->throbberLabel->movie()->start();
        ui->throbberLabel->show();
        ui->blockListTreeWidget->setEnabled(false);
        ui->savePushButton->setEnabled(false);
    } else {
        ui->throbberLabel->movie()->stop();
        ui->throbberLabel->hide();
        ui->blockListTreeWidget->setEnabled(true);
        ui->savePushButton->setEnabled(true);
    }
}

void QtBlockListEditorWindow::setError(const std::string& error) {
    if (!error.empty()) {
        ui->errorLabel->setText("<font color='red'>" + QtUtilities::htmlEscape(P2QSTRING(error)) + "</font>");
    }
    else {
        ui->errorLabel->setText("");
    }
}

std::vector<JID> Swift::QtBlockListEditorWindow::getCurrentBlockList() const {
    std::vector<JID> futureBlockedJIDs;

    for(int i=0; i < ui->blockListTreeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* row = ui->blockListTreeWidget->topLevelItem(i);
        JID jid = JID(Q2PSTRING(row->text(0)));
        if (!jid.toString().empty() && jid.isValid()) {
            futureBlockedJIDs.push_back(jid);
        }
    }
    return futureBlockedJIDs;
}

bool QtBlockListEditorWindow::eventFilter(QObject* target, QEvent* event) {
    if (target == ui->blockListTreeWidget) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Backspace) {
                // remove currently selected item
                QTreeWidgetItem* currentItem = ui->blockListTreeWidget->currentItem();
                if (currentItem->text(0) != freshBlockListTemplate) {
                    ui->blockListTreeWidget->takeTopLevelItem(ui->blockListTreeWidget->indexOfTopLevelItem(currentItem));
                    return true;
                }
            }
            else if (keyEvent->key() == Qt::Key_Return) {
                // open editor for return key d
                ui->blockListTreeWidget->editItem(ui->blockListTreeWidget->currentItem(), 0);
                return true;
            }
        }
    }
    return QWidget::eventFilter(target, event);
}

}
