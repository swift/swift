/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtVCardURLField.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QTextDocument>
#include <boost/algorithm/string.hpp>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardURLField::QtVCardURLField(QWidget* parent, QGridLayout *layout, bool editable) :
	QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("URL"), false, false) {
	connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardURLField::~QtVCardURLField() {
	disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardURLField::setupContentWidgets() {
	urlLabel = new QLabel(this);
	urlLabel->setOpenExternalLinks(true);
	urlLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
	urlLineEdit = new QtResizableLineEdit(this);

	QHBoxLayout* urlLayout = new QHBoxLayout();
	urlLayout->addWidget(urlLabel);
	urlLayout->addWidget(urlLineEdit);

	getGridLayout()->addLayout(urlLayout, getGridLayout()->rowCount()-1, 2, 1, 2, Qt::AlignVCenter);
	getTagComboBox()->hide();
	urlLabel->hide();
	childWidgets << urlLabel << urlLineEdit;
}

bool QtVCardURLField::isEmpty() const {
	return urlLineEdit->text().isEmpty();
}

void QtVCardURLField::setURL(const std::string& url) {
	urlLineEdit->setText(P2QSTRING(url));
}

std::string QtVCardURLField::getURL() const {
	return Q2PSTRING(urlLineEdit->text());
}

void QtVCardURLField::handleEditibleChanged(bool isEditable) {
	if (isEditable) {
		if (urlLineEdit) urlLineEdit->show();
		if (urlLabel) urlLabel->hide();
	} else {
		if (urlLineEdit) urlLineEdit->hide();
		if (urlLabel) {
			urlLabel->setText(QString("<a href=\"%1\">%1</a>").arg(Qt::escape(urlLineEdit->text())));
			urlLabel->show();
		}
	}
}

}
