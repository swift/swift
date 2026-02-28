/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWidget>

namespace Swift {
    class QtElidingLabel;
    class SettingsProvider;

    class QtNameWidget : public QWidget {
            Q_OBJECT

        public:
            QtNameWidget(SettingsProvider* settings, QWidget *parent);

            void setNick(const QString& text);
            void setJID(const QString& jid);
            void setOnline(const bool isOnline);

        signals:
            void onChangeNickRequest();

        private:
            void updateText();
            virtual void mousePressEvent(QMouseEvent* event);

        private:
            enum Mode {
                ShowNick,
                ShowJID
            };

            SettingsProvider* settings;
            Mode mode;
            QtElidingLabel* textLabel;
            QString jid;
            QString nick;
            bool isOnline_;
    };
}

