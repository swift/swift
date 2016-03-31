/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/FileTransfer.h>

using namespace Swift;

FileTransfer::FileTransfer() : fileSizeInBytes_(0), state_(State::Initial) {
}

FileTransfer::~FileTransfer() {
}

void FileTransfer::setState(const State& state) {
    state_ = state;
    onStateChanged(state);
}

void FileTransfer::setFileInfo(const std::string& name, boost::uintmax_t size, const std::string& description) {
    filename_ = name;
    fileSizeInBytes_ = size;
    description_ = description;
}
