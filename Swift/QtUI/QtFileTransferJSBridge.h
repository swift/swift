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

class QtFileTransferJSBridge : public QObject {
	 Q_OBJECT
public:
	QtFileTransferJSBridge();
	virtual ~QtFileTransferJSBridge();
signals:
	void discard(QString id);
	void sendRequest(QString id);
	void setDescription(QString id);
	void acceptRequest(QString id, QString filename);
	void cancel(QString id);
};

}
