/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/cstdint.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/FileTransfer/FileTransferError.h>

namespace Swift {

class FileTransfer {
public:
	struct State {
		enum FTState {
			Canceled,
			Failed,
			Finished,
			Negotiating,
			Transferring,
			WaitingForStart,
			WaitingForAccept,
		};

		FTState state;
		std::string message;

		State(FTState state) : state(state), message("") {}
		State(FTState state, std::string message) : state(state), message(message) {}
	};

public:
	typedef boost::shared_ptr<FileTransfer> ref;

public:
	boost::uintmax_t fileSizeInBytes;
	std::string filename;
	std::string algo;
	std::string hash;

public:
	virtual void cancel() = 0;

public:
	boost::signal<void (int /* proccessedBytes */)> onProcessedBytes;
	boost::signal<void (State)> onStateChange;
	boost::signal<void (boost::optional<FileTransferError>)> onFinished;

public:
	virtual ~FileTransfer() {}
};

}
