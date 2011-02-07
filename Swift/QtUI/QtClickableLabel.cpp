/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtClickableLabel.h"

namespace Swift {

QtClickableLabel::QtClickableLabel(QWidget* parent) : QLabel(parent) {
}

void QtClickableLabel::mousePressEvent(QMouseEvent*) {
	emit clicked();
}

}
