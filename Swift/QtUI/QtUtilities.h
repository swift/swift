/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

class QWidget;
class QString;
class QDateTime;

#include <QKeyEvent>

namespace QtUtilities {
    void setX11Resource(QWidget* widget, const QString& c);
    QString htmlEscape(const QString& s);
    #ifdef SWIFTEN_PLATFORM_MACOSX
    const Qt::KeyboardModifier ctrlHardwareKeyModifier = Qt::MetaModifier;
    #else
    const Qt::KeyboardModifier ctrlHardwareKeyModifier = Qt::ControlModifier;
    #endif

    /**
     * @brief secondsToNextMidnight calculates the seconds until next midnight.
     * @param currentTime This is the current time, which SHOULD have a Qt::TimeSpec
     * of Qt::LocalTime to correctly handle DST changes in the current locale.
     * @return
     */
    long long secondsToNextMidnight(const QDateTime& currentTime);
}
