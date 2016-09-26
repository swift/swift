/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QObject>

namespace Swift {
    /**
     * This class is here as a workaround for a bug in Qt.
     * See Case #501.
     */
    class CocoaApplicationActivateHelper : public QObject {
        public:
            CocoaApplicationActivateHelper();
            virtual ~CocoaApplicationActivateHelper();

        private:
            bool eventFilter(QObject* o, QEvent* e);

        private:
            struct Private;
            const std::unique_ptr<Private> p;
    };
}
