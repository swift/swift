/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtTabbable.h"

#include <QApplication>

namespace Swift {

QtTabbable::~QtTabbable() {
	emit windowClosing();
}

bool QtTabbable::isWidgetSelected() {
	/*isActiveWindow() shouldn't be necessary, but I don't trust it as far as I can throw it*/
	return isActiveWindow() && isAncestorOf(QApplication::focusWidget());
}

}
