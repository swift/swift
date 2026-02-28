/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/signals2/connection.hpp>

#include <QDialog>

class QLabel;
class QProgressBar;

namespace Swift {
    class AutoUpdater;
    class SettingsProvider;

    class QtAboutWidget : public QDialog {
            Q_OBJECT

        public:
            QtAboutWidget(SettingsProvider* settings, AutoUpdater* autoUpdater);

        private slots:
            void handleLicenseClicked();
            void handleChangelogClicked();
            void handleChangeUpdateChannelClicked();

        private:
            void openPlainTextWindow(const QString& path);
            void updateUpdateInfo();

        protected:
            void showEvent(QShowEvent*);

        private:
            SettingsProvider* settingsProvider_;
            AutoUpdater* autoUpdater_;
            QLabel* updateChannelInfoLabel_ = nullptr;
            QLabel* updateStateInfoLabel_ = nullptr;
            QProgressBar* updateProgressBar_ = nullptr;
            boost::signals2::scoped_connection settingsChangedConnection_;
            boost::signals2::scoped_connection autoUpdaterChangeConnection_;
    };
}
