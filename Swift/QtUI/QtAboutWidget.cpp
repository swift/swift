/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/QtAboutWidget.h"

#include <QCoreApplication>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include <QtGlobal>
#include <QPushButton>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>

namespace Swift {

QtAboutWidget::QtAboutWidget() : QDialog() {
#ifndef Q_WS_MAC
	setWindowTitle(QString(tr("About %1")).arg("Swift"));
#endif
	setWindowIcon(QIcon(":/logo-icon-16.png"));

	resize(180, 240);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setAlignment(Qt::AlignHCenter);
	setLayout(mainLayout);
	
	QLabel* iconLabel = new QLabel(this);
	iconLabel->setPixmap(QIcon(":/logo-shaded-text.256.png").pixmap(90, 90));
	iconLabel->setAlignment(Qt::AlignHCenter);
	mainLayout->addWidget(iconLabel);
	
	QLabel* appNameLabel = new QLabel("<center><font size='+1'><b>" + QCoreApplication::applicationName() + "</b></font></center>", this);
	mainLayout->addWidget(appNameLabel);
	
	QLabel* versionLabel = new QLabel(QString("<center><font size='-1'>") + tr("Version %1").arg(QCoreApplication::applicationVersion()) + "</font></center>", this);
	mainLayout->addWidget(versionLabel);
	QString buildString = QString("<center><font size='-1'>") + QString(tr("Built with Qt %1")).arg(QT_VERSION_STR);
	buildString += QString("<br/>") + QString(tr("Running with Qt %1")).arg(qVersion());
	buildString += "</font></center>";
	QLabel* buildLabel = new QLabel(buildString, this);
	mainLayout->addWidget(buildLabel);

	QPushButton* licenseButton = new QPushButton(tr("View License"), this);
	mainLayout->addWidget(licenseButton);
	connect(licenseButton, SIGNAL(clicked()), this, SLOT(handleLicenseClicked()));

	setFixedSize(minimumSizeHint());
}

void QtAboutWidget::handleLicenseClicked() {
	QTextEdit* text = new QTextEdit();
	text->setAttribute(Qt::WA_DeleteOnClose);
	text->setReadOnly(true);
	QFile file(":/COPYING");
	file.open(QIODevice::ReadOnly);
	QTextStream in(&file);
	in.setCodec("UTF-8");
	text->setPlainText(in.readAll());
	file.close();
	text->resize(500, 600);
	text->show();
	text->activateWindow();
}

}
