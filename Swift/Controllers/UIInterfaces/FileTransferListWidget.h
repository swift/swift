/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

namespace Swift {

class FileTransferOverview;

class FileTransferListWidget {
public:
	virtual ~FileTransferListWidget() {}

	virtual void show() = 0;
	virtual void activate() = 0;

	virtual void setFileTransferOverview(FileTransferOverview*) = 0;
};

}
