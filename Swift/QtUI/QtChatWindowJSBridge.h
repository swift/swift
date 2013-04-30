/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QObject>

#include <map>

namespace Swift {

class FileTransferController;

class QtChatWindowJSBridge : public QObject {
	 Q_OBJECT
public:
	QtChatWindowJSBridge();
	virtual ~QtChatWindowJSBridge();
signals:
	void buttonClicked(QString id, QString arg1, QString arg2, QString arg3, QString arg4, QString arg5);
};

}
