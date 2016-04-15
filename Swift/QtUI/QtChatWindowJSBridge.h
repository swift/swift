/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <QObject>

namespace Swift {

class FileTransferController;

class QtChatWindowJSBridge : public QObject {
     Q_OBJECT
public:
    QtChatWindowJSBridge();
    virtual ~QtChatWindowJSBridge();
signals:
    void buttonClicked(QString id, QString arg1, QString arg2, QString arg3, QString arg4, QString arg5);
    void verticalScrollBarPositionChanged(double scrollbarPosition);
};

}
