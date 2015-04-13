/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "QtCertificateViewerDialog.h"
#include "ui_QtCertificateViewerDialog.h"

#include <Swiften/Base/foreach.h>

#include <QDateTime>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <QTreeWidgetItem>

namespace Swift {

QtCertificateViewerDialog::QtCertificateViewerDialog(QWidget* parent) : QDialog(parent), ui(new Ui::QtCertificateViewerDialog) {
	ui->setupUi(this);
	connect(ui->certChainTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

	setAttribute(Qt::WA_DeleteOnClose);
}

QtCertificateViewerDialog::~QtCertificateViewerDialog() {
	delete ui;
}

void QtCertificateViewerDialog::setCertificateChain(const std::vector<Certificate::ref>& chain) {
	// clean widgets
	ui->certChainTreeWidget->clear();

	if (chain.empty()) return;

	// convert Swift certificate chain to qt certificate chain (root goes first)
	currentChain.clear();
	foreach(Certificate::ref cert, chain) {
		ByteArray certAsDer = cert->toDER();
		QByteArray dataArray(reinterpret_cast<const char*>(certAsDer.data()), certAsDer.size());
		currentChain.push_front(QSslCertificate(dataArray, QSsl::Der));
	}

	// fill treeWidget
	QTreeWidgetItem* root = new QTreeWidgetItem(ui->certChainTreeWidget, QStringList(currentChain.at(0).subjectInfo(QSslCertificate::CommonName)));
	root->setData(0, Qt::UserRole, QVariant(0));
	root->setExpanded(true);
	ui->certChainTreeWidget->addTopLevelItem(root);
	if (currentChain.size() > 1) {
		QTreeWidgetItem* parent = root;
		for (int n = 1; n < currentChain.size(); n++) {
			QTreeWidgetItem* link = new QTreeWidgetItem(parent, QStringList(QString("↳ ") + (QStringList(currentChain.at(n).subjectInfo(QSslCertificate::CommonName)).join(", "))));
			link->setExpanded(true);
			link->setData(0, Qt::UserRole, QVariant(n));
			parent = link;
		}
		ui->certChainTreeWidget->setCurrentItem(parent);
	} else {
		ui->certChainTreeWidget->setCurrentItem(root);
	}
}

void QtCertificateViewerDialog::displayCertificateChainAsSheet(QWidget* parent, const std::vector<Certificate::ref>& chain) {
	QtCertificateViewerDialog* dialog = new QtCertificateViewerDialog(parent);
	dialog->setCertificateChain(chain);
	dialog->show();
}

void QtCertificateViewerDialog::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*) {
	setCertificateDetails(currentChain.at(current->data(0, Qt::UserRole).toInt()));
}

#define ADD_SECTION( TITLE ) \
	ui->certGridLayout->addWidget(new QLabel("<strong>" + TITLE + "</strong>"), rowCount++, 0, 1, 2);

#define ADD_FIELD( TITLE, VALUE) \
	ui->certGridLayout->addWidget(new QLabel(TITLE), rowCount, 0, 1, 1, Qt::AlignRight); \
	valueLabel = new QLabel(); \
	valueLabel->setText(QStringList(VALUE).join(", ")); \
	valueLabel->setTextFormat(Qt::PlainText); \
	valueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard); \
	ui->certGridLayout->addWidget(valueLabel, rowCount++, 1, 1, 1, Qt::AlignLeft);

void QtCertificateViewerDialog::setCertificateDetails(const QSslCertificate& cert) {
	QLayoutItem* item;
	while ((item = ui->certGridLayout->takeAt(0)) != NULL ) {
		delete item->widget();
		delete item;
	}

	int rowCount = 0;

	ui->certGridLayout->setColumnStretch(2, 1);

	QLabel* valueLabel = 0;

	ADD_SECTION(tr("General"));
	ADD_FIELD(tr("Valid From"), cert.effectiveDate().toString(Qt::TextDate));
	ADD_FIELD(tr("Valid To"), cert.expiryDate().toString(Qt::TextDate));
	ADD_FIELD(tr("Serial Number"), QString(cert.serialNumber().toHex()));
	ADD_FIELD(tr("Version"), QString(cert.version()));

	ADD_SECTION(tr("Subject"));
	ADD_FIELD(tr("Organization"), cert.subjectInfo(QSslCertificate::Organization));
	ADD_FIELD(tr("Common Name"), cert.subjectInfo(QSslCertificate::CommonName));
	ADD_FIELD(tr("Locality"), cert.subjectInfo(QSslCertificate::LocalityName));
	ADD_FIELD(tr("Organizational Unit"), cert.subjectInfo(QSslCertificate::OrganizationalUnitName));
	ADD_FIELD(tr("Country"), cert.subjectInfo(QSslCertificate::CountryName));
	ADD_FIELD(tr("State"), cert.subjectInfo(QSslCertificate::StateOrProvinceName));

#if QT_VERSION < 0x050000
	QMultiMap<QSsl::AlternateNameEntryType, QString> altNames = cert.alternateSubjectNames();
#define SANTYPE QSsl::AlternateNameEntryType
#else
	QMultiMap<QSsl::AlternativeNameEntryType, QString> altNames = cert.subjectAlternativeNames();
#define SANTYPE QSsl::AlternativeNameEntryType
#endif
	if (!altNames.empty()) {
		ADD_SECTION(tr("Alternate Subject Names"));
		foreach (const SANTYPE &type, altNames.uniqueKeys()) {
			foreach (QString name, altNames.values(type)) {
				if (type == QSsl::EmailEntry) {
					ADD_FIELD(tr("E-mail Address"), name);
				} else {
					ADD_FIELD(tr("DNS Name"), name);
				}
			}
		}
	}

	ADD_SECTION(tr("Issuer"));
	ADD_FIELD(tr("Organization"), cert.issuerInfo(QSslCertificate::Organization));
	ADD_FIELD(tr("Common Name"), cert.issuerInfo(QSslCertificate::CommonName));
	ADD_FIELD(tr("Locality"), cert.issuerInfo(QSslCertificate::LocalityName));
	ADD_FIELD(tr("Organizational Unit"), cert.issuerInfo(QSslCertificate::OrganizationalUnitName));
	ADD_FIELD(tr("Country"), cert.issuerInfo(QSslCertificate::CountryName));
	ADD_FIELD(tr("State"), cert.issuerInfo(QSslCertificate::StateOrProvinceName));

	ui->certGridLayout->setRowStretch(rowCount + 1, 1);
}

}
