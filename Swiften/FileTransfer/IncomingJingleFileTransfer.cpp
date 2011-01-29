/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>

namespace Swift {

IncomingJingleFileTransfer::IncomingJingleFileTransfer(IncomingJingleSession::ref session) : session(session) {

}

void IncomingJingleFileTransfer::accept(WriteBytestream::ref stream) {
	this->stream = stream;
}

}
