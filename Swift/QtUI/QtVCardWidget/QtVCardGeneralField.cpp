/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtVCardGeneralField.h"

#include <QHBoxLayout>
#include <Swiften/Base/Log.h>

namespace Swift {

QtVCardGeneralField::QtVCardGeneralField(QWidget* parent, QGridLayout* layout, bool editable, int row, QString label, bool preferrable, bool taggable) :
	QWidget(parent), preferrable(preferrable), taggable(taggable), layout(layout), row(row), preferredCheckBox(0), label(0), labelText(label),
	tagComboBox(0), closeButton(0) {
	setEditable(editable);
}

QtVCardGeneralField::~QtVCardGeneralField() {

}

void QtVCardGeneralField::initialize() {
	if (preferrable) {
		preferredCheckBox = new QCheckBox(this);
		preferredCheckBox->setStyleSheet(
					"QCheckBox::indicator { width: 18px; height: 18px; }"
					"QCheckBox::indicator:checked { image: url(:/icons/star-checked.png); }"
					"QCheckBox::indicator:unchecked { image: url(:/icons/star-unchecked); }"
			);
		layout->addWidget(preferredCheckBox, row, 0, Qt::AlignVCenter);
		childWidgets << preferredCheckBox;
	}
	label = new QLabel(this);
	label->setText(labelText);
	layout->addWidget(label, row, 1, Qt::AlignVCenter | Qt::AlignRight);

	tagLabel = new QLabel(this);
	tagLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

	tagComboBox = new QtTagComboBox(this);
	closeButton = new QtCloseButton(this);
	connect(closeButton, SIGNAL(clicked()), SLOT(handleCloseButtonClicked()));

	QHBoxLayout* tagLayout = new QHBoxLayout();
	tagLayout->addWidget(tagLabel);
	tagLayout->addWidget(tagComboBox);

	setupContentWidgets();
	layout->addLayout(tagLayout, row, 4, Qt::AlignTop);
	layout->addWidget(closeButton, row, 5, Qt::AlignCenter);
	closeButton->resize(12, 12);
	tagLabel->hide();

	childWidgets << label << tagComboBox << tagLabel << closeButton;
}

bool QtVCardGeneralField::isEditable() const {
	return editable;
}

void QtVCardGeneralField::setEditable(bool editable) {
	this->editable = editable;
	if (tagComboBox) {
		if (taggable) {
			tagLabel->setText(tagComboBox->itemText(0));
			tagComboBox->setVisible(editable);
			tagLabel->setVisible(!editable);
		} else {
			tagLabel->hide();
			tagComboBox->hide();
		}
	}
	if (closeButton) closeButton->setVisible(editable);
	if (preferredCheckBox) {
		if (editable) {
			preferredCheckBox->show();
		} else if (!preferredCheckBox->isChecked()) {
			preferredCheckBox->hide();
		}
		preferredCheckBox->setEnabled(editable);
	}
	editableChanged(this->editable);
}

void QtVCardGeneralField::setPreferred(const bool preferred) {
	if (preferredCheckBox) preferredCheckBox->setChecked(preferred);
}

bool QtVCardGeneralField::getPreferred() const {
	return preferredCheckBox ? preferredCheckBox->isChecked() : false;
}

void QtVCardGeneralField::customCleanup() {
}

QtTagComboBox* QtVCardGeneralField::getTagComboBox() const {
	return tagComboBox;
}

QGridLayout* QtVCardGeneralField::getGridLayout() const {
	return layout;
}

void QtVCardGeneralField::handleCloseButtonClicked() {
	customCleanup();
	foreach(QWidget* widget, childWidgets) {
		widget->hide();
		layout->removeWidget(widget);
	}
	deleteField(this);
}

}
