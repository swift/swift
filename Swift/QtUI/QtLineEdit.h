/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QLineEdit>

namespace Swift {
    class QtLineEdit : public QLineEdit {
        Q_OBJECT
        public:
            QtLineEdit(QWidget* parent = nullptr);
        signals:
            void escapePressed();
        protected:
            virtual void keyPressEvent(QKeyEvent* event);
    };
}
