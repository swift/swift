/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtConnectionSettingsWindow.h>

#include <boost/lexical_cast.hpp>

#include <QCoreApplication>
#include <QFile>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QtGlobal>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtURLValidator.h>

namespace Swift {

QtConnectionSettingsWindow::QtConnectionSettingsWindow(const ClientOptions& options) : QDialog() {
    ui.setupUi(this);

    connect(ui.connectionMethod, SIGNAL(currentIndexChanged(int)), ui.stackedWidget, SLOT(setCurrentIndex(int)));

    connect(ui.manual_manualHost, SIGNAL(toggled(bool)), ui.manual_manualHostNameLabel, SLOT(setEnabled(bool)));
    connect(ui.manual_manualHost, SIGNAL(toggled(bool)), ui.manual_manualHostName, SLOT(setEnabled(bool)));
    connect(ui.manual_manualHost, SIGNAL(toggled(bool)), ui.manual_manualHostPortLabel, SLOT(setEnabled(bool)));
    connect(ui.manual_manualHost, SIGNAL(toggled(bool)), ui.manual_manualHostPort, SLOT(setEnabled(bool)));

    connect(ui.manual_manualProxy, SIGNAL(toggled(bool)), ui.manual_manualProxyHostLabel, SLOT(setEnabled(bool)));
    connect(ui.manual_manualProxy, SIGNAL(toggled(bool)), ui.manual_manualProxyHost, SLOT(setEnabled(bool)));
    connect(ui.manual_manualProxy, SIGNAL(toggled(bool)), ui.manual_manualProxyPortLabel, SLOT(setEnabled(bool)));
    connect(ui.manual_manualProxy, SIGNAL(toggled(bool)), ui.manual_manualProxyPort, SLOT(setEnabled(bool)));

    connect(ui.bosh_manualProxy, SIGNAL(toggled(bool)), ui.bosh_manualProxyHostLabel, SLOT(setEnabled(bool)));
    connect(ui.bosh_manualProxy, SIGNAL(toggled(bool)), ui.bosh_manualProxyHost, SLOT(setEnabled(bool)));
    connect(ui.bosh_manualProxy, SIGNAL(toggled(bool)), ui.bosh_manualProxyPortLabel, SLOT(setEnabled(bool)));
    connect(ui.bosh_manualProxy, SIGNAL(toggled(bool)), ui.bosh_manualProxyPort, SLOT(setEnabled(bool)));

    connect(ui.manual_proxyType, SIGNAL(currentIndexChanged(int)), SLOT(handleProxyTypeChanged(int)));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(handleAcceptRequested()));

    QtURLValidator* urlValidator = new QtURLValidator(this);
    ui.bosh_uri->setValidator(urlValidator);

    ui.manual_useTLS->setCurrentIndex(2);

    ui.manual_proxyType->setCurrentIndex(0);

    ClientOptions defaults;
    if (options.boshURL.isEmpty()) {
        bool isDefault = options.useStreamCompression == defaults.useStreamCompression;
        isDefault &= options.useTLS == defaults.useTLS;
        isDefault &= options.allowPLAINWithoutTLS == defaults.allowPLAINWithoutTLS;
        isDefault &= options.useStreamCompression == defaults.useStreamCompression;
        isDefault &= options.useAcks == defaults.useAcks;
        isDefault &= options.manualHostname == defaults.manualHostname;
        isDefault &= options.manualPort == defaults.manualPort;
        isDefault &= options.proxyType == defaults.proxyType;
        isDefault &= options.manualProxyHostname == defaults.manualProxyHostname;
        isDefault &= options.manualProxyPort == defaults.manualProxyPort;
        isDefault &= options.tlsOptions.schannelTLS1_0Workaround == defaults.tlsOptions.schannelTLS1_0Workaround;
        if (isDefault) {
            ui.connectionMethod->setCurrentIndex(0);
        }
        else {
            ui.connectionMethod->setCurrentIndex(1);
            ui.manual_useTLS->setCurrentIndex(options.useTLS);
            ui.manual_allowPLAINWithoutTLS->setChecked(options.allowPLAINWithoutTLS);
            ui.manual_allowCompression->setChecked(options.useStreamCompression);
            if (!options.manualHostname.empty()) {
                ui.manual_manualHost->setChecked(true);
                ui.manual_manualHostName->setText(P2QSTRING(options.manualHostname));
                if (options.manualPort >=0) {
                    ui.manual_manualHostPort->setText(P2QSTRING(boost::lexical_cast<std::string>(options.manualPort)));
                }
            }
            ui.manual_proxyType->setCurrentIndex(options.proxyType);
            if (!options.manualProxyHostname.empty()) {
                ui.manual_manualProxy->setChecked(true);
                ui.manual_manualProxyHost->setText(P2QSTRING(options.manualProxyHostname));
                ui.manual_manualProxyPort->setText(P2QSTRING(boost::lexical_cast<std::string>(options.manualProxyPort)));
            }
            ui.manual_forceTLS1_0->setChecked(options.tlsOptions.schannelTLS1_0Workaround);
        }
    } else {
        ui.connectionMethod->setCurrentIndex(2);
        ui.bosh_uri->setText(P2QSTRING(options.boshURL.toString()));
        if (!options.boshHTTPConnectProxyURL.isEmpty()) {
            ui.bosh_manualProxy->setChecked(true);
            ui.bosh_manualProxyHost->setText(P2QSTRING(options.boshHTTPConnectProxyURL.getHost()));
            if (options.boshHTTPConnectProxyURL.getPort()) {
                ui.bosh_manualProxyPort->setText(P2QSTRING(boost::lexical_cast<std::string>(*options.boshHTTPConnectProxyURL.getPort())));
            }
        }
    }
#ifndef HAVE_SCHANNEL
    ui.manual_forceTLS1_0->hide();
#endif
}

void QtConnectionSettingsWindow::handleProxyTypeChanged(int index) {
    bool proxySettingsVisible = index != NoProxy && index != SystemProxy;
    ui.manual_manualProxy->setVisible(proxySettingsVisible);
    ui.manual_manualProxyHostLabel->setVisible(proxySettingsVisible);
    ui.manual_manualProxyHost->setVisible(proxySettingsVisible);
    ui.manual_manualProxyPortLabel->setVisible(proxySettingsVisible);
    ui.manual_manualProxyPort->setVisible(proxySettingsVisible);
}

void QtConnectionSettingsWindow::handleAcceptRequested() {
    if (ui.connectionMethod->currentIndex() != 2 || ui.bosh_uri->hasAcceptableInput()) {
        accept();
    }
    else {
        QMessageBox::critical(this, tr("Configuration invalid"), tr("The provided BOSH URL is not valid."));
    }
}

ClientOptions QtConnectionSettingsWindow::getOptions() {
    ClientOptions options;
    if (ui.connectionMethod->currentIndex() > 0) {
        /* Not automatic */
        if (ui.connectionMethod->currentIndex() == 1) {
            /* Manual */
            options.useTLS = static_cast<ClientOptions::UseTLS>(ui.manual_useTLS->currentIndex());
            options.useStreamCompression = ui.manual_allowCompression->isChecked();
            options.allowPLAINWithoutTLS = ui.manual_allowPLAINWithoutTLS->isChecked();
            options.tlsOptions.schannelTLS1_0Workaround = ui.manual_forceTLS1_0->isChecked();
            if (ui.manual_manualHost->isChecked()) {
                options.manualHostname = Q2PSTRING(ui.manual_manualHostName->text());
                try {
                    options.manualPort = boost::lexical_cast<int>(Q2PSTRING(ui.manual_manualHostPort->text()));
                } catch (const boost::bad_lexical_cast&) {
                    options.manualPort = -1;
                }
            }
            options.proxyType = static_cast<ClientOptions::ProxyType>(ui.manual_proxyType->currentIndex());
            if (ui.manual_manualProxy->isChecked()) {
                options.manualProxyHostname = Q2PSTRING(ui.manual_manualProxyHost->text());
                try {
                    options.manualProxyPort = boost::lexical_cast<int>(Q2PSTRING(ui.manual_manualProxyPort->text()));
                } catch (const boost::bad_lexical_cast&) {}
            }
        }
        else {
            /* BOSH */
            options.boshURL = URL::fromString(Q2PSTRING(ui.bosh_uri->text()));
            if (ui.bosh_manualProxy->isChecked()) {
                std::string host = Q2PSTRING(ui.bosh_manualProxyHost->text());
                try {
                    int port = boost::lexical_cast<int>(Q2PSTRING(ui.bosh_manualProxyPort->text()));
                    options.boshHTTPConnectProxyURL = URL("http", host, port, "");
                } catch (const boost::bad_lexical_cast&) {
                    options.boshHTTPConnectProxyURL = URL("http", host, "");
                }
            }
        }
    }
    return options;
}

}
