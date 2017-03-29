/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <fstream>
#include <map>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/Debug.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/sleep.h>
#include <Swiften/Base/StdRandomGenerator.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/Disco/ClientDiscoManager.h>
#include <Swiften/Disco/EntityCapsProvider.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/FileTransfer/FileReadBytestream.h>
#include <Swiften/FileTransfer/FileTransferManager.h>
#include <Swiften/FileTransfer/FileWriteBytestream.h>
#include <Swiften/FileTransfer/OutgoingFileTransfer.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>

using namespace Swift;

static const std::string CLIENT_NAME = "Swiften FT Test";
static const std::string CLIENT_NODE = "http://swift.im";

static std::shared_ptr<SimpleEventLoop> eventLoop;
static std::shared_ptr<BoostNetworkFactories> networkFactories;

StdRandomGenerator randGen;

enum Candidate {
    InBandBytestream = 1,
    S5B_Direct = 2,
    S5B_Proxied = 4,
    S5B_Assisted = 8,
};

class ConcurrentFileTransferTest {
    public:
        ConcurrentFileTransferTest(int clientACandidates, int clientBCandidates) : clientACandidates_(clientACandidates), clientBCandidates_(clientBCandidates) {

        }

    private:
        int clientACandidates_;
        std::shared_ptr<Client> clientA_;
        std::map<std::string, ByteArray> clientASendFiles_;


        int clientBCandidates_;
        std::shared_ptr<Client> clientB_;
};

/**
 *    This program tests the concurrent transfer of multiple file-transfers.
 *
 */
int main(int argc, char** argv) {
    int failedTests = 0;

    if (!env("SWIFT_FILETRANSFERTEST_JID") && !env("SWIFT_FILETRANSFERTEST_PASS") && !env("SWIFT_FILETRANSFERTEST2_JID") && !env("SWIFT_FILETRANSFERTEST2_PASS")) {

        return -1;
    }


    return failedTests;
}
