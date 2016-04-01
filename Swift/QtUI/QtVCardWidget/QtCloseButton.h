/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QAbstractButton>

namespace Swift {

    class QtCloseButton : public QAbstractButton {
            Q_OBJECT
        public:
            explicit QtCloseButton(QWidget *parent = nullptr);
            virtual QSize sizeHint() const;

        protected:
            virtual bool event(QEvent *e);
            virtual void paintEvent(QPaintEvent* );
    };

}
