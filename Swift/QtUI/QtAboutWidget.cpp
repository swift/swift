/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtAboutWidget.h>

#include <QCoreApplication>
#include <QFile>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QtGlobal>

#include <Swiften/Base/Platform.h>

namespace Swift {

QtAboutWidget::QtAboutWidget() : QDialog() {
#ifndef Q_OS_MAC
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

    QLabel* versionLabel = new QLabel((QString("<center><font size='-1'>") + tr("Version %1") + "</font></center><center><font size='-1'><br/>" + QString(tr("Built with Qt %2")) + QString("<br/>") + QString(tr("Running with Qt %3")) + "</font></center>").arg(QCoreApplication::applicationVersion()).arg(QT_VERSION_STR).arg(qVersion()));
    versionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    mainLayout->addWidget(versionLabel);

    if (QCoreApplication::translate("TRANSLATION_INFO", "TRANSLATION_AUTHOR") != "TRANSLATION_AUTHOR") {
        mainLayout->addWidget(new QLabel(QString("<center><font size='-1'>") + QString(tr("Using the English translation by\n%1")).arg(QCoreApplication::translate("TRANSLATION_INFO", "TRANSLATION_AUTHOR")).replace("\n", "<br/>") + "</font></center>", this));
    }
    QCoreApplication::translate("TRANSLATION_INFO", "TRANSLATION_LICENSE", "This string contains the license under which this translation is licensed. We ask you to license the translation under the BSD license. Please read http://www.opensource.org/licenses/bsd-license.php, and if you agree to release your translation under this license, use the following (untranslated) text: 'This translation is licensed under the BSD License. See http://www.opensource.org/licenses/bsd-license.php'");
#if defined(SWIFTEN_PLATFORM_WINDOWS) || defined(SWIFTEN_PLATFORM_MACOSX)
    QPushButton* licenseButton = new QPushButton(tr("View License"), this);
    mainLayout->addWidget(licenseButton);
    connect(licenseButton, SIGNAL(clicked()), this, SLOT(handleLicenseClicked()));
#else
    // Some Linux desktops have dialog window decorations without close window buttons.
    // This code adds a dedicated button to close the about window dialog.
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    mainLayout->addLayout(buttonLayout);

    QPushButton* licenseButton = new QPushButton(tr("View License"), this);
    buttonLayout->addWidget(licenseButton);
    connect(licenseButton, SIGNAL(clicked()), this, SLOT(handleLicenseClicked()));

    buttonLayout->addItem(new QSpacerItem(20,20));

    QPushButton* closeButton = new QPushButton(tr("Close"), this);
    buttonLayout->addWidget(closeButton);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
#endif
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
