/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <QtBlockListEditorWindow.h>
#include <ui_QtBlockListEditorWindow.h>

#include <boost/bind.hpp>

#include <QLineEdit>
#include <QMovie>
#include <QShortcut>
#include <QStyledItemDelegate>
#include <QValidator>

#include <Swift/QtUI/QtUtilities.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Base/foreach.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swiften/JID/JID.h>

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

QtBlockListEditorWindow::QtBlockListEditorWindow() : QWidget(), ui(new Ui::QtBlockListEditorWindow) {
	ui->setupUi(this);
	new QShortcut(QKeySequence::Close, this, SLOT(close()));
	ui->throbberLabel->setMovie(new QMovie(":/icons/throbber.gif", QByteArray(), this));

	itemDelegate = new QtRemovableItemDelegate(style());

	connect(ui->savePushButton, SIGNAL(clicked()), SLOT(applyChanges()));

	ui->blockListTreeWidget->setColumnCount(2);
	ui->blockListTreeWidget->header()->setStretchLastSection(false);
	int closeIconWidth = style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, 0, 0);
	ui->blockListTreeWidget->header()->resizeSection(1, closeIconWidth);

#if QT_VERSION >= 0x050000
	ui->blockListTreeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
	ui->blockListTreeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
#endif

	ui->blockListTreeWidget->setHeaderHidden(true);
	ui->blockListTreeWidget->setRootIsDecorated(false);
	ui->blockListTreeWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui->blockListTreeWidget->setItemDelegateForColumn(0, new QtJIDValidatedItemDelegate(this));
	ui->blockListTreeWidget->setItemDelegateForColumn(1, itemDelegate);
	connect(ui->blockListTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(handleItemChanged(QTreeWidgetItem*,int)));

	QTreeWidgetItem* item = new QTreeWidgetItem(QStringList("") << "");
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	ui->blockListTreeWidget->addTopLevelItem(item);
}

QtBlockListEditorWindow::~QtBlockListEditorWindow() {
}

void QtBlockListEditorWindow::show() {
	QWidget::show();
	QWidget::activateWindow();
}

void QtBlockListEditorWindow::handleItemChanged(QTreeWidgetItem *, int) {
	bool hasEmptyRow = false;
	QList<QTreeWidgetItem*> rows = ui->blockListTreeWidget->findItems("", Qt::MatchFixedString);
	foreach(QTreeWidgetItem* row, rows) {
		if (row->text(0).isEmpty()) {
			hasEmptyRow = true;
		}
	}

	if (!hasEmptyRow) {
		QTreeWidgetItem* item = new QTreeWidgetItem(QStringList("") << "");
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		ui->blockListTreeWidget->addTopLevelItem(item);
	}
}

void QtBlockListEditorWindow::applyChanges() {
	onSetNewBlockList(getCurrentBlockList());
}

void Swift::QtBlockListEditorWindow::setCurrentBlockList(const std::vector<JID> &blockedJIDs) {
	ui->blockListTreeWidget->clear();

	foreach(const JID& jid, blockedJIDs) {
		QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(P2QSTRING(jid.toString())) << "");
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		ui->blockListTreeWidget->addTopLevelItem(item);
	}
	handleItemChanged(0,0);
}

void Swift::QtBlockListEditorWindow::setBusy(bool isBusy) {
	if (isBusy) {
		ui->throbberLabel->movie()->start();
		ui->throbberLabel->show();
	} else {
		ui->throbberLabel->movie()->stop();
		ui->throbberLabel->hide();
	}
}

std::vector<JID> Swift::QtBlockListEditorWindow::getCurrentBlockList() const {
	std::vector<JID> futureBlockedJIDs;

	for(int i=0; i < ui->blockListTreeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem* row = ui->blockListTreeWidget->topLevelItem(i);
		if (!row->text(0).isEmpty()) {
			futureBlockedJIDs.push_back(JID(Q2PSTRING(row->text(0))));
		}
	}
	return futureBlockedJIDs;
}

}
