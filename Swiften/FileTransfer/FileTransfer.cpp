/*
 * Copyright (c) 2013-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FileTransfer.h>

using namespace Swift;

FileTransfer::FileTransfer() : fileSizeInBytes(0) {
}

FileTransfer::~FileTransfer() {
}

void FileTransfer::setFileInfo(const std::string& name, boost::uintmax_t size) {
	filename = name;
	fileSizeInBytes = size;
}
