/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QObject>

#include <SwifTools/URIHandler/URIHandler.h>

class QUrl;

namespace Swift {
    class QtURIHandler : public QObject, public URIHandler {
        public:
            QtURIHandler();

        private:
            bool eventFilter(QObject* obj, QEvent* event);
    };
}
