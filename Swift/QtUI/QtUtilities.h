/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

class QWidget;
class QString;

namespace QtUtilities {
	void setX11Resource(QWidget* widget, const QString& c);
};
