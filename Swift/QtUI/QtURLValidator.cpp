/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtURLValidator.h>

#include <Swiften/Base/URL.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
QtURLValidator::QtURLValidator(QObject* parent) {

}

QValidator::State QtURLValidator::validate(QString& input, int& pos) const {
	URL url = URL::fromString(Q2PSTRING(input));
	bool valid = !url.isEmpty();
	valid &= (url.getScheme() == "http" || url.getScheme() == "https");
	return valid ? Acceptable : Intermediate;
}

}

