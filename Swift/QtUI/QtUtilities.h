/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
