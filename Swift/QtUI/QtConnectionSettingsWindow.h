/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QDialog>

#include <Swiften/Client/ClientOptions.h>

#include <Swift/QtUI/ui_QtConnectionSettings.h>

namespace Swift {
    class QtConnectionSettingsWindow : public QDialog {
            Q_OBJECT

        public:
            QtConnectionSettingsWindow(const ClientOptions& options);

            ClientOptions getOptions();

        private slots:
            void handleProxyTypeChanged(int);
            void handleAcceptRequested();

        private:
            enum {
                NoProxy = 0,
                SystemProxy = 1,
                SOCKS5Proxy = 2,
                HTTPProxy = 3
            };
            Ui::QtConnectionSettings ui;
    };
}
