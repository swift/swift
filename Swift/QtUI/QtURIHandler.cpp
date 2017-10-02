/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtURIHandler.h>

#include <QCoreApplication>
#include <QFileOpenEvent>
#include <QUrl>

#include <Swift/QtUI/QtSwiftUtil.h>

using namespace Swift;

QtURIHandler::QtURIHandler() {
    qApp->installEventFilter(this);
}

bool QtURIHandler::eventFilter(QObject*, QEvent* event) {
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent* fileOpenEvent = static_cast<QFileOpenEvent*>(event);
        if (fileOpenEvent->url().scheme() == "xmpp") {
            onURI(Q2PSTRING(fileOpenEvent->url().toString()));
            return true;
        }
    }
    return false;
}
