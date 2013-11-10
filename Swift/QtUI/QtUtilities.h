/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

class QWidget;
class QString;

#include <QKeyEvent>

namespace QtUtilities {
	void setX11Resource(QWidget* widget, const QString& c);
	QString htmlEscape(const QString& s);
	#ifdef SWIFTEN_PLATFORM_MACOSX
	const Qt::KeyboardModifier ctrlHardwareKeyModifier = Qt::MetaModifier;
	#else
	const Qt::KeyboardModifier ctrlHardwareKeyModifier = Qt::ControlModifier;
	#endif
}
