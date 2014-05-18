/*
 * Copyright (c) 2013-2014 Remko Tronçon
 * Licensed under the GNU General Public License.
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
