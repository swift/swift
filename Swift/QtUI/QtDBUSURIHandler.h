/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QObject>

#include <SwifTools/URIHandler/URIHandler.h>

namespace Swift {
    class QtDBUSURIHandler : public QObject, public URIHandler {
        public:
            QtDBUSURIHandler();
    };
}
