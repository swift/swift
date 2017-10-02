/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QCheckBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QPointer>
#include <QPushButton>
#include <QStackedWidget>

#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/LoginWindow.h>
#include <Swift/Controllers/UIInterfaces/MainWindow.h>

#include <Swift/QtUI/QtAboutWidget.h>

class QLabel;
class QToolButton;
class QComboBox;

namespace Swift {
    class AutoUpdater;
    class SettingsProvider;
    class TimerFactory;

    class QtLoginWindow : public QMainWindow, public LoginWindow {
        Q_OBJECT
        public:
            struct QtMenus {
                QtMenus(QMenu* swiftMenu, QMenu* generalMenu) : swiftMenu(swiftMenu), generalMenu(generalMenu) {}
                QMenu* swiftMenu;
                QMenu* generalMenu;
            };

        public:
            QtLoginWindow(UIEventStream* uiEventStream, SettingsProvider* settings, TimerFactory* timerFactory, AutoUpdater* autoUpdater);

            void morphInto(MainWindow *mainWindow);
            virtual void loggedOut();
            virtual void setShowNotificationToggle(bool);
            virtual void setMessage(const std::string& message);
            virtual void addAvailableAccount(const std::string& defaultJID, const std::string& defaultPassword, const std::string& defaultCertificate, const ClientOptions& options);
            virtual void removeAvailableAccount(const std::string& jid);
            virtual void setLoginAutomatically(bool loginAutomatically);
            virtual void setIsLoggingIn(bool loggingIn);
            void selectUser(const std::string& user);
            bool askUserToTrustCertificatePermanently(const std::string& message, const std::vector<Certificate::ref>& certificate);
            void hide();
            QtMenus getMenus() const;
            virtual void quit();

        signals:
            void geometryChanged();

        private slots:
            void loginClicked();
            void handleCertficateChecked(bool);
            void handleQuit();
            void handleShowXMLConsole();
            void handleShowFileTransferOverview();
            void handleShowHighlightEditor();
            void handleToggleSounds(bool enabled);
            void handleToggleNotifications(bool enabled);
            void handleAbout();
            void bringToFront();
            void toggleBringToFront();
            void handleUsernameTextChanged();
            void resizeEvent(QResizeEvent* event);
            void moveEvent(QMoveEvent* event);
            void handleSettingChanged(const std::string& settingPath);
            void handleOpenConnectionOptions();

        protected:
            bool eventFilter(QObject *obj, QEvent *event);

        private:
            void setInitialMenus();
            QWidget* loginWidgetWrapper_;
            QStringList usernames_;
            QStringList passwords_;
            QStringList certificateFiles_;
            std::vector<ClientOptions> options_;
            QComboBox* username_;
            QLineEdit* password_;
            QPushButton* loginButton_;
            /* If you add a widget here, change setLoggingIn as well.*/
            QCheckBox* remember_;
            QCheckBox* loginAutomatically_;
            QStackedWidget* stack_;
            QLabel* message_;
            QString certificateFile_;
            QToolButton* certificateButton_;
            QMenuBar* menuBar_;
            QMenu* swiftMenu_;
            QMenu* generalMenu_;
            QMenu* viewMenu_ = nullptr;
            QAction* toggleSoundsAction_;
            QAction* toggleNotificationsAction_;
            UIEventStream* uiEventStream_;
            QPointer<QtAboutWidget> aboutDialog_;
            SettingsProvider* settings_;
            QAction* xmlConsoleAction_;
            QAction* fileTransferOverviewAction_;
            QAction* highlightEditorAction_;
            TimerFactory* timerFactory_;
            ClientOptions currentOptions_;
            AutoUpdater* autoUpdater_;
    };
}
