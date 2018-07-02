/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QListView>

#include <Swift/QtUI/ServerList/ServerListDelegate.h>

namespace Swift {
    class QtServerListView : public QListView {
        Q_OBJECT
        public:
            QtServerListView();
            virtual ~QtServerListView();
        private:
            std::unique_ptr<ServerListDelegate> delegate_;
            static const int widgetWidth = 82;
    };
}
