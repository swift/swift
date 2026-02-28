/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <QValidator>

namespace Swift {
    class QtURLValidator : public QValidator {
        Q_OBJECT
        public:
            QtURLValidator(QObject* parent);
            virtual QValidator::State validate(QString& input, int& pos) const;
    };
}

