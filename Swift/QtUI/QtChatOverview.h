/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWidget>

#include <Swiften/JID/JID.h>

namespace Swift {
    class Chattables;
    class ChattablesModel;
    class QtChatOverview : public QWidget {
            Q_OBJECT
        public:
            QtChatOverview(Chattables&,  QWidget* parent);
            ~QtChatOverview() override;

        private slots:
            void handleItemClicked(JID jid);
        private:
            Chattables& chattables_;
            ChattablesModel* rootModel_;
    };
}
