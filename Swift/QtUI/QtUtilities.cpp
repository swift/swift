/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "QtUtilities.h"

#include <QtGui>
#if defined (Q_OS_UNIX) && !defined(Q_OS_MAC)
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#include <QTextDocument>
#include <QWidget>

#include "Swift/Controllers/ApplicationInfo.h"

namespace QtUtilities {


void setX11Resource(QWidget* widget, const QString& c) {
#if defined (Q_OS_UNIX) && !defined(Q_OS_MAC) && QT_VERSION < 0x050000
	char res_class[] = SWIFT_APPLICATION_NAME;
	XClassHint hint;
	QByteArray resName = (QString(SWIFT_APPLICATION_NAME) + "-" + c).toUtf8();
	hint.res_name = resName.data();
	hint.res_class = res_class;
	XSetClassHint(widget->x11Info().display(), widget->winId(), &hint);
#else
	(void) widget;
	(void) c;
#endif
}

QString htmlEscape(const QString& s) {
#if QT_VERSION >= 0x050000
	return s.toHtmlEscaped();
#else
	return Qt::escape(s);
#endif
}

}
