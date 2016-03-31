/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QLineEdit>

namespace Swift {
    class QtLineEdit : public QLineEdit {
        Q_OBJECT
        public:
            QtLineEdit(QWidget* parent = NULL);
        signals:
            void escapePressed();
        protected:
            virtual void keyPressEvent(QKeyEvent* event);
    };
}
