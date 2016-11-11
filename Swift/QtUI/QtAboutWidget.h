/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2/connection.hpp>

#include <QDialog>

class QLabel;

namespace Swift {
    class SettingsProvider;

    class QtAboutWidget : public QDialog {
            Q_OBJECT

        public:
            QtAboutWidget(SettingsProvider* settings);

        private slots:
            void handleLicenseClicked();
            void handleChangelogClicked();
            void handleChangeUpdateChannelClicked();

        private:
            void openPlainTextWindow(const QString& path);
            void updateUpdateInfoLabel();

        private:
            SettingsProvider* settingsProvider_;
            QLabel* updateInfoLabel_ = nullptr;
            boost::signals2::scoped_connection settingsChangedConnection_;
    };
}
