/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtURLValidator.h>

#include <Swiften/Base/URL.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
QtURLValidator::QtURLValidator(QObject* parent) : QValidator(parent) {

}

QValidator::State QtURLValidator::validate(QString& input, int&) const {
    URL url = URL::fromString(Q2PSTRING(input));
    bool valid = !url.isEmpty();
    valid &= (url.getScheme() == "http" || url.getScheme() == "https");
    return valid ? Acceptable : Intermediate;
}

}

