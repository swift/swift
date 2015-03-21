/*
 * Copyright (c) 2012-2014 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/QtUI/QtVCardWidget/QtVCardOrganizationField.h>

#include <boost/algorithm/string.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardOrganizationField::QtVCardOrganizationField(QWidget* parent, QGridLayout *layout, bool editable) :
	QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("Organization"), false, false), organizationLabel(NULL), organizationLineEdit(NULL), unitsTreeWidget(NULL), itemDelegate(NULL) {
	connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardOrganizationField::~QtVCardOrganizationField() {
	disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardOrganizationField::setupContentWidgets() {
	organizationLabel = new QLabel(this);
	organizationLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	organizationLineEdit = new QtResizableLineEdit(this);
	QHBoxLayout* organizationLayout = new QHBoxLayout();
	organizationLayout->addWidget(organizationLabel);
	organizationLayout->addWidget(organizationLineEdit);

	getGridLayout()->addLayout(organizationLayout, getGridLayout()->rowCount()-1, 2, 1, 2, Qt::AlignVCenter);

	itemDelegate = new QtRemovableItemDelegate(style());

	unitsTreeWidget = new QTreeWidget(this);
	connect(unitsTreeWidget->model(), SIGNAL(rowsRemoved(QModelIndex, int, int)), SLOT(handleRowsRemoved(QModelIndex,int,int)));
	unitsTreeWidget->setColumnCount(2);
	unitsTreeWidget->header()->setStretchLastSection(false);
	unitsTreeWidget->header()->resizeSection(1, itemDelegate->sizeHint(QStyleOptionViewItem(), QModelIndex()).width());

#if QT_VERSION >= 0x050000
	unitsTreeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
	unitsTreeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
#endif

	unitsTreeWidget->setHeaderHidden(true);
	unitsTreeWidget->setRootIsDecorated(false);
	unitsTreeWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	unitsTreeWidget->setItemDelegateForColumn(1, itemDelegate);
	connect(unitsTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(handleItemChanged(QTreeWidgetItem*,int)));
	getGridLayout()->addWidget(unitsTreeWidget, getGridLayout()->rowCount()-1, 4, 2, 1);

	QTreeWidgetItem* item = new QTreeWidgetItem(QStringList("") << "");
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	unitsTreeWidget->addTopLevelItem(item);

	getTagComboBox()->hide();
	organizationLabel->hide();
	childWidgets << organizationLabel << organizationLineEdit << unitsTreeWidget;
}

bool QtVCardOrganizationField::isEmpty() const {
	return organizationLineEdit->text().isEmpty() && unitsTreeWidget->model()->rowCount() != 0;
}

void QtVCardOrganizationField::setOrganization(const VCard::Organization& organization) {
	organizationLineEdit->setText(P2QSTRING(organization.name));
	unitsTreeWidget->clear();
	foreach(std::string unit, organization.units) {
		QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(P2QSTRING(unit)) << "");
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		unitsTreeWidget->addTopLevelItem(item);
	}

	QTreeWidgetItem* item = new QTreeWidgetItem(QStringList("") << "");
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	unitsTreeWidget->addTopLevelItem(item);
}

VCard::Organization QtVCardOrganizationField::getOrganization() const {
	VCard::Organization organization;
	organization.name = Q2PSTRING(organizationLineEdit->text());
	for(int i=0; i < unitsTreeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem* row = unitsTreeWidget->topLevelItem(i);
		if (!row->text(0).isEmpty()) {
			organization.units.push_back(Q2PSTRING(row->text(0)));
		}
	}

	return organization;
}

void QtVCardOrganizationField::handleEditibleChanged(bool isEditable) {
	assert(organizationLineEdit);
	assert(unitsTreeWidget);

	organizationLineEdit->setVisible(isEditable);
	organizationLabel->setVisible(!isEditable);

	if (!isEditable) {
		QString label;
		for(int i=0; i < unitsTreeWidget->topLevelItemCount(); ++i) {
			QTreeWidgetItem* row = unitsTreeWidget->topLevelItem(i);
			if (!row->text(0).isEmpty()) {
				label += row->text(0) + ", ";
			}
		}
		label += organizationLineEdit->text();
		organizationLabel->setText(label);
	}
	unitsTreeWidget->setVisible(isEditable);
}

void QtVCardOrganizationField::handleItemChanged(QTreeWidgetItem *, int) {
	guaranteeEmptyRow();
}

void QtVCardOrganizationField::handleRowsRemoved(const QModelIndex&, int, int) {
	guaranteeEmptyRow();
}

void QtVCardOrganizationField::guaranteeEmptyRow() {
	bool hasEmptyRow = false;
	QList<QTreeWidgetItem*> rows = unitsTreeWidget->findItems("", Qt::MatchFixedString);
	foreach(QTreeWidgetItem* row, rows) {
		if (row->text(0).isEmpty()) {
			hasEmptyRow = true;
		}
	}

	if (!hasEmptyRow) {
		QTreeWidgetItem* item = new QTreeWidgetItem(QStringList("") << "");
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		unitsTreeWidget->addTopLevelItem(item);
		unitsTreeWidget->setCurrentItem(item);
	}
}

}
