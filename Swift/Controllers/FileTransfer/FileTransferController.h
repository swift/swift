/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/cstdint.hpp>

#include <Swiften/FileTransfer/FileReadBytestream.h>
#include <Swiften/FileTransfer/FileTransfer.h>
#include <Swiften/FileTransfer/FileWriteBytestream.h>
#include <Swiften/FileTransfer/IncomingFileTransfer.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/FileTransfer/FileTransferProgressInfo.h>

namespace Swift {

class FileTransferManager;
class ChatWindow;

class FileTransferController {
public:
    /**
     * For outgoing file transfers. It'll create a file transfer via FileTransferManager as soon as the descriptive information is available.
     */
    FileTransferController(const JID&, const std::string&, FileTransferManager*);

    /**
     * For incoming file transfers.
     */
    FileTransferController(IncomingFileTransfer::ref transfer);
    ~FileTransferController();

    std::string setChatWindow(ChatWindow*, const std::string& nickname, const std::string& avatarPath);
    void setReceipient(const JID& otherParty);

    void start(std::string& description);
    void accept(std::string& file);
    void cancel();

    const JID &getOtherParty() const;
    bool isIncoming() const;
    FileTransfer::State getState() const;
    int getProgress() const;
    boost::uintmax_t getSize() const;

    boost::signals2::signal<void ()> onStateChanged;
    boost::signals2::signal<void ()> onProgressChange;

private:
    void handleFileTransferStateChange(FileTransfer::State);
    void handleProgressPercentageChange(int percentage);

private:
    bool sending;
    JID otherParty;
    std::string filename;
    FileTransfer::ref transfer;
    std::shared_ptr<FileReadBytestream> fileReadStream;
    std::shared_ptr<FileWriteBytestream> fileWriteStream;
    FileTransferManager* ftManager;
    FileTransferProgressInfo* ftProgressInfo;
    ChatWindow* chatWindow;
    std::string uiID;
    FileTransfer::State currentState;
};

}
