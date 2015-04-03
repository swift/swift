/*
 * Copyright (c) 2012-2014 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtVCardWidget/QtVCardGeneralField.h>

#include <cassert>

#include <QHBoxLayout>
#include <QToolTip>

#include <Swift/QtUI/QtElidingLabel.h>

namespace Swift {

QtVCardGeneralField::QtVCardGeneralField(QWidget* parent, QGridLayout* layout, bool editable, int row, QString label, bool preferrable, bool taggable) :
	QWidget(parent), editable(editable), preferrable(preferrable), starVisible(false), taggable(taggable), layout(layout), row(row), preferredCheckBox(0), label(0), labelText(label),
	tagComboBox(0), tagLabel(NULL), closeButton(0) {
}

QtVCardGeneralField::~QtVCardGeneralField() {

}

void QtVCardGeneralField::initialize() {
	if (preferrable) {
		preferredCheckBox = new QCheckBox(this);
		preferredCheckBox->setToolTip(tr("Stars can be used to mark preferred contact details."));
		preferredCheckBox->setStyleSheet(
					"QCheckBox::indicator { width: 18px; height: 18px; }"
					"QCheckBox::indicator:checked { image: url(:/icons/star-checked.png); }"
					"QCheckBox::indicator:unchecked { image: url(:/icons/star-unchecked); }"
			);
		layout->addWidget(preferredCheckBox, row, 0, Qt::AlignVCenter);
		childWidgets << preferredCheckBox;
		connect(preferredCheckBox, SIGNAL(stateChanged(int)), SLOT(handlePreferredStarStateChanged(int)));
	}
	label = new QLabel(this);
	label->setText(labelText);
	layout->addWidget(label, row, 1, Qt::AlignVCenter | Qt::AlignRight);

	tagLabel = new QtElidingLabel(this);
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
	setEditable(editable);
}

bool QtVCardGeneralField::isEditable() const {
	return editable;
}

void QtVCardGeneralField::setEditable(bool editable) {
	assert(tagComboBox);
	assert(closeButton);

	this->editable = editable;
	if (taggable) {
		tagLabel->setText(tagComboBox->itemText(0));
		tagComboBox->setVisible(editable);
		tagLabel->setVisible(!editable);
	} else {
		tagLabel->hide();
		tagComboBox->hide();
	}
	closeButton->setVisible(editable);
	updatePreferredStarVisibility();
	editableChanged(this->editable);
}

void QtVCardGeneralField::setStarVisible(const bool isVisible) {
	starVisible = isVisible;
	updatePreferredStarVisibility();
}

bool QtVCardGeneralField::getStarVisible() const {
	return starVisible;
}

void QtVCardGeneralField::setPreferred(const bool preferred) {
	if (preferredCheckBox) preferredCheckBox->setChecked(preferred);
	updatePreferredStarVisibility();
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

void QtVCardGeneralField::handlePreferredStarStateChanged(int state) {
	if (state == Qt::Checked) {
		QToolTip::showText(QCursor::pos(), tr("Marked as your preferred %1. Click again to undo.").arg(labelText));
	}
}

void QtVCardGeneralField::updatePreferredStarVisibility() {
	if (preferredCheckBox) {
		bool showStar = false;
		if (editable) {
			if (starVisible) {
				showStar = true;
			}
			else {
				showStar = preferredCheckBox->isChecked();
			}
		}
		else {
			showStar = preferredCheckBox->isChecked();
		}
		preferredCheckBox->setVisible(showStar);
		preferredCheckBox->setEnabled(editable);
	}
}

}
