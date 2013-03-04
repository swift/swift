/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtVCardJIDField.h"

#include <QGridLayout>
#include <QTextDocument>
#include <boost/algorithm/string.hpp>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>

namespace Swift {

QtVCardJIDField::QtVCardJIDField(QWidget* parent, QGridLayout *layout, bool editable) :
	QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("JID"), false, false) {
	connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardJIDField::~QtVCardJIDField() {
	disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardJIDField::setupContentWidgets() {
	jidLabel = new QLabel(this);
	jidLabel->setOpenExternalLinks(true);
	jidLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
	jidLineEdit = new QtResizableLineEdit(this);
#if QT_VERSION >= 0x040700
	jidLineEdit->setPlaceholderText(tr("alice@wonderland.lit"));
#endif
	QHBoxLayout* jidLayout = new QHBoxLayout();
	jidLayout->addWidget(jidLabel);
	jidLayout->addWidget(jidLineEdit);
	getGridLayout()->addLayout(jidLayout, getGridLayout()->rowCount()-1, 2, 1, 2, Qt::AlignVCenter);

	jidLabel->hide();
	getTagComboBox()->hide();

	childWidgets << jidLabel << jidLineEdit;
}

bool QtVCardJIDField::isEmpty() const {
	return jidLineEdit->text().isEmpty();
}

void QtVCardJIDField::setJID(const JID& jid) {
	std::string jidStr = jid.toBare().toString();
	jidLineEdit->setText(P2QSTRING(jidStr));
}

JID QtVCardJIDField::getJID() const {
	return JID(Q2PSTRING(jidLineEdit->text()));
}

void QtVCardJIDField::handleEditibleChanged(bool isEditable) {
	if (isEditable) {
		if (jidLineEdit) jidLineEdit->show();
		if (jidLabel) jidLabel->hide();
	} else {
		if (jidLineEdit) jidLineEdit->hide();
		if (jidLabel) {
			jidLabel->setText(QString("<a href=\"xmpp:%1\">%1</a>").arg(QtUtilities::htmlEscape(jidLineEdit->text())));
			jidLabel->show();
		}
	}
}

}
