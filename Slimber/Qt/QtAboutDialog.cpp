/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Slimber/Qt/QtAboutDialog.h"

#include <QCoreApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

QtAboutDialog::QtAboutDialog() {
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle("About Slimber");

	QVBoxLayout* layout = new QVBoxLayout(this);
	
	QLabel* iconLabel = new QLabel(this);
	iconLabel->setPixmap(QPixmap(":/icons/Icon-128.png"));
	iconLabel->setAlignment(Qt::AlignHCenter);
	layout->addWidget(iconLabel);
	
	QLabel* appNameLabel = new QLabel("<center><font size='+1'><b>" + QCoreApplication::applicationName() + "</b></font></center>", this);
	layout->addWidget(appNameLabel);
	
	QLabel* versionLabel = new QLabel(QString("<center><font size='-1'>Version ") + QCoreApplication::applicationVersion() + "</font></center>", this);
	layout->addWidget(versionLabel);
	QString buildString = QString("<center><font size='-1'>Built with: Qt version ") + QT_VERSION_STR;
	buildString += QString("<br/>Running with Qt version ") + qVersion();
	buildString += "</font></center>";
	QLabel* buildLabel = new QLabel(buildString, this);
	layout->addWidget(buildLabel);

	setFixedSize(minimumSizeHint());
}
