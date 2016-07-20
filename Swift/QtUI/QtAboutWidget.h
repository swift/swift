/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QDialog>

namespace Swift {
    class QtAboutWidget : public QDialog {
            Q_OBJECT

        public:
            QtAboutWidget();

        private slots:
            void handleLicenseClicked();
            void handleChangelogClicked();

        private:
            void openPlainTextWindow(const QString& path);
    };
}
