/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <fstream>

#include <boost/algorithm/string.hpp>
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

class FileTransferTest {
    public:
        FileTransferTest(int senderCandidates, int receiverCandidates) : senderCandidates_(senderCandidates), senderError_(FileTransferError::UnknownError), senderIsDone_(false), receiverCandidates_(receiverCandidates), receiverError_(FileTransferError::UnknownError), receiverIsDone_(false) {
            sender_ = std::make_shared<Client>(JID(getenv("SWIFT_FILETRANSFERTEST_JID")), getenv("SWIFT_FILETRANSFERTEST_PASS"), networkFactories.get());
            sender_->onDisconnected.connect(boost::bind(&FileTransferTest::handleSenderDisconnected, this, _1));
            sender_->onConnected.connect(boost::bind(&FileTransferTest::handleSenderConnected, this));
            sender_->getEntityCapsProvider()->onCapsChanged.connect(boost::bind(&FileTransferTest::handleSenderCapsChanged, this, _1));

            receiver_ = std::make_shared<Client>(JID(getenv("SWIFT_FILETRANSFERTEST2_JID")), getenv("SWIFT_FILETRANSFERTEST2_PASS"), networkFactories.get());
            receiver_->onConnected.connect(boost::bind(&FileTransferTest::handleReceiverConnected, this));
            receiver_->onDisconnected.connect(boost::bind(&FileTransferTest::handleReceiverDisconnected, this, _1));

            senderTracer_ = new ClientXMLTracer(sender_.get());
            receiverTracer_ = new ClientXMLTracer(receiver_.get());

            ClientOptions options;
            options.useTLS = ClientOptions::NeverUseTLS;
            options.useStreamCompression = false;
            options.useStreamResumption = false;
            options.useAcks = false;

            sender_->connect(options);
            receiver_->connect(options);

            timeOut_ = networkFactories->getTimerFactory()->createTimer(60000);
            timeOut_->onTick.connect(boost::bind(&FileTransferTest::handleTimeOut, this));

            // Create randomly sized data to exchange.
            sendFilePath_ = boost::filesystem::unique_path("ft_send_%%%%%%%%%%%%%%%%.bin");
            receiveFilePath_ = boost::filesystem::unique_path("ft_receive_%%%%%%%%%%%%%%%%.bin");

            size_t size = 1024 + boost::numeric_cast<size_t>(randGen.generateRandomInteger(1024 * 10));
            sendData_.resize(size);
            for (unsigned char& n : sendData_) {
                n = boost::numeric_cast<unsigned char>(randGen.generateRandomInteger(255));
            }

            std::ofstream outfile(sendFilePath_.native().c_str(), std::ios::out | std::ios::binary);
            outfile.write(reinterpret_cast<char *>(&sendData_[0]), boost::numeric_cast<ptrdiff_t>(sendData_.size()));
            outfile.close();
        }

        ~FileTransferTest() {
            timeOut_->stop();

            delete senderTracer_;
            delete receiverTracer_;

            // Free file-transfer objects so file handles are closed and files can be removed afterwards.
            assert(!outgoingFileTransfer_ && incomingFileTransfers_.empty());

            if(boost::filesystem::exists(sendFilePath_)) {
                boost::filesystem::remove(sendFilePath_);
            }

            if(boost::filesystem::exists(receiveFilePath_)) {
                boost::filesystem::remove(receiveFilePath_);
            }
        }

        void handleSenderConnected() {
            DiscoInfo discoInfo;
            discoInfo.addIdentity(DiscoInfo::Identity(CLIENT_NAME, "client", "pc"));
            discoInfo.addFeature(DiscoInfo::JingleFeature);
            discoInfo.addFeature(DiscoInfo::JingleFTFeature);
            discoInfo.addFeature(DiscoInfo::Bytestream);
            if (senderCandidates_ & InBandBytestream) {
                discoInfo.addFeature(DiscoInfo::JingleTransportsIBBFeature);
            }
            if (senderCandidates_ & (S5B_Direct | S5B_Assisted | S5B_Proxied)) {
                discoInfo.addFeature(DiscoInfo::JingleTransportsS5BFeature);
            }
            sender_->getDiscoManager()->setCapsNode(CLIENT_NODE);
            sender_->getDiscoManager()->setDiscoInfo(discoInfo);
            sender_->sendPresence(Presence::create());
        }

        void handleReceiverConnected() {
            receiver_->getFileTransferManager()->onIncomingFileTransfer.connect(boost::bind(&FileTransferTest::handleReceiverIncomingFileTransfer, this, _1));

            DiscoInfo discoInfo;
            discoInfo.addIdentity(DiscoInfo::Identity(CLIENT_NAME, "client", "pc"));
            discoInfo.addFeature(DiscoInfo::JingleFeature);
            discoInfo.addFeature(DiscoInfo::JingleFTFeature);
            discoInfo.addFeature(DiscoInfo::Bytestream);
            if (receiverCandidates_ & InBandBytestream) {
                discoInfo.addFeature(DiscoInfo::JingleTransportsIBBFeature);
            }
            if (receiverCandidates_ & (S5B_Direct | S5B_Assisted | S5B_Proxied)) {
                discoInfo.addFeature(DiscoInfo::JingleTransportsS5BFeature);
            }
            receiver_->getDiscoManager()->setCapsNode(CLIENT_NODE);
            receiver_->getDiscoManager()->setDiscoInfo(discoInfo);
            receiver_->getPresenceSender()->sendPresence(Presence::create());
        }

        void handleReceiverIncomingFileTransfer(IncomingFileTransfer::ref transfer) {
            incomingFileTransfers_.push_back(transfer);
            std::shared_ptr<FileWriteBytestream> out = std::make_shared<FileWriteBytestream>(receiveFilePath_.native());
            transfer->onFinished.connect(boost::bind(&FileTransferTest::handleReceiverFileTransferFinished, this, _1, out));

            FileTransferOptions options;
            options = options.withInBandAllowed(receiverCandidates_ & InBandBytestream);
            options = options.withDirectAllowed(receiverCandidates_ & S5B_Direct);
            options = options.withAssistedAllowed(receiverCandidates_ & S5B_Assisted);
            options = options.withProxiedAllowed(receiverCandidates_ & S5B_Proxied);

            std::cout << "Incoming transfer options: " << "IBB (" << options.isInBandAllowed() << ")" << ", ";
            std::cout << "S5B Direct (" << options.isDirectAllowed() << ")" << ", ";
            std::cout << "S5B Assisted (" << options.isAssistedAllowed() << ")" << ", ";
            std::cout << "S5B Proxied (" << options.isProxiedAllowed() << ")" << std::endl;

            transfer->accept(out, options);
        }

        void handleSenderCapsChanged(const JID &jid) {
            if (receiver_ && (receiver_->getJID().toBare() == jid.toBare())) {
                std::shared_ptr<FileReadBytestream> fileStream = std::make_shared<FileReadBytestream>(sendFilePath_);

                FileTransferOptions options;
                options = options.withInBandAllowed(senderCandidates_ & InBandBytestream);
                options = options.withDirectAllowed(senderCandidates_ & S5B_Direct);
                options = options.withAssistedAllowed(senderCandidates_ & S5B_Assisted);
                options = options.withProxiedAllowed(senderCandidates_ & S5B_Proxied);

                std::cout << "Outgoing transfer options: " << "IBB (" << options.isInBandAllowed() << ")" << ", ";
                std::cout << "S5B Direct (" << options.isDirectAllowed() << ")" << ", ";
                std::cout << "S5B Assisted (" << options.isAssistedAllowed() << ")" << ", ";
                std::cout << "S5B Proxied (" << options.isProxiedAllowed() << ")" << std::endl;

                outgoingFileTransfer_ = sender_->getFileTransferManager()->createOutgoingFileTransfer(jid.toBare(), sendFilePath_, "Some File!", fileStream, options);

                if (outgoingFileTransfer_) {
                    outgoingFileTransfer_->onFinished.connect(boost::bind(&FileTransferTest::handleSenderFileTransferFinished, this, _1));
                    outgoingFileTransfer_->start();
                } else {
                    std::cout << "ERROR: No outgoing file transfer returned." << std::endl;
                    receiverIsDone_ = true;
                    senderIsDone_ = true;
                    endTest();
                }
            }
        }

        void handleReceiverFileTransferFinished(const boost::optional<FileTransferError>& error, std::shared_ptr<FileWriteBytestream> out) {
            out->close();
            receiverError_ = error;
            receiverIsDone_ = true;
            if (senderIsDone_) {
                timeOut_->stop();
                timeOut_ = networkFactories->getTimerFactory()->createTimer(1000);
                timeOut_->onTick.connect(boost::bind(&FileTransferTest::endTest, this));
                timeOut_->start();
            }
        }

        void handleSenderDisconnected(const boost::optional<ClientError>& error) {
            if (error) {
                std::cout << this << " " << "handleSenderDisconnected: error: " << error.get() << std::endl;
            }

            // All file-transfers related to a Client instance need to be freed
            // *before* freeing the Client instance.
            outgoingFileTransfer_.reset();

            sender_.reset();
            if (!sender_ && !receiver_) {
                eventLoop->stop();
            }
        }

        void handleReceiverDisconnected(const boost::optional<ClientError>& error) {
            if (error) {
                std::cout << this << " " << "handleReceiverDisconnected: error: " << error.get() << std::endl;
            }

            // All file-transfers related to a Client instance need to be freed
            // *before* freeing the Client instance.
            incomingFileTransfers_.clear();

            receiver_.reset();
            if (!sender_ && !receiver_) {
                eventLoop->stop();
            }
        }

        void handleSenderFileTransferFinished(const boost::optional<FileTransferError>& error) {
            senderError_ = error;
            senderIsDone_ = true;
            if (receiverIsDone_) {
                timeOut_->stop();
                timeOut_ = networkFactories->getTimerFactory()->createTimer(1000);
                timeOut_->onTick.connect(boost::bind(&FileTransferTest::endTest, this));
                timeOut_->start();
            }
            else if (error) {
                endTest();
            }
        }

        void run() {
            timeOut_->start();
            eventLoop->run();
        }

        void endTest() {
            if (sender_) {
                sender_->disconnect();
            }
            if (receiver_) {
                receiver_->disconnect();
            }
        }

        void handleTimeOut() {
            std::cout << "Test timed out!!!" << std::endl;
            endTest();
        }

        bool isDone() const {
            return senderIsDone_ && receiverIsDone_;
        }

        bool wasSuccessful() const {
            return !senderError_ && !receiverError_;
        }

    private:
        int senderCandidates_;
        std::shared_ptr<Client> sender_;
        ClientXMLTracer* senderTracer_;
        ByteArray sendData_;
        OutgoingFileTransfer::ref outgoingFileTransfer_;
        boost::filesystem::path sendFilePath_;
        boost::optional<FileTransferError> senderError_;
        bool senderIsDone_;

        int receiverCandidates_;
        std::shared_ptr<Client> receiver_;
        ClientXMLTracer* receiverTracer_;
        ByteArray receiveData_;
        std::vector<IncomingFileTransfer::ref> incomingFileTransfers_;
        boost::filesystem::path receiveFilePath_;
        boost::optional<FileTransferError> receiverError_;
        bool receiverIsDone_;

        Timer::ref timeOut_;
};

static bool runTest(int senderCandidates, int receiverCandidates) {
    bool success = false;

    std::cout << "senderCandidates: " << senderCandidates << ", receiverCandidates: " << receiverCandidates << std::endl;
    bool expectSuccess = (senderCandidates & receiverCandidates) > 0;

    eventLoop = std::make_shared<SimpleEventLoop>();
    networkFactories = std::make_shared<BoostNetworkFactories>(eventLoop.get());

    std::shared_ptr<FileTransferTest> testRun = std::make_shared<FileTransferTest>(senderCandidates, receiverCandidates);

    testRun->run();

    bool wasSuccessful = testRun->wasSuccessful();
    if (expectSuccess == wasSuccessful) {
        success = true;
    }
    else {
        if (!testRun->isDone()) {
            std::cout << "Test did not finish transfer. Sender candidates = " << senderCandidates << ", receiver candidates = " << receiverCandidates << "." << std::endl;
        }
    }
    std::cout << "expected success: " << expectSuccess << ", wasSuccessful: " << wasSuccessful << std::endl;


    testRun.reset();
    networkFactories.reset();
    eventLoop->runUntilEvents();

    eventLoop->stop();
    eventLoop.reset();

    return success;
}

/**
 *    This program test file-transfer     interop between Swift and itself with various connection candidates.
 *    The all combinations of the candidates, IBB, S5B (direct) and S5B (proxied), on sender and receiver side are tested.
 */
int main(int argc, char** argv) {
    int failedTests = 0;

    std::vector<std::pair<int, int> > failedTestPairs;
    std::cout << "Swiften File-Transfer Connectivity Test Suite" << std::endl;
    if (argc == 1) {
        if (getenv("SWIFT_FILETRANSFERTEST_CONFIG")) {
            // test configuration described in SWIFT_FILETRANSFERTEST_CONFIG environment variable, e.g. "1:1|2:2"
            std::vector<std::string> configurations;
            std::string configs_env = std::string(getenv("SWIFT_FILETRANSFERTEST_CONFIG"));
            boost::split(configurations, configs_env, boost::is_any_of("|"));
            for (const auto& config : configurations) {
                std::vector<std::string> split_config;
                boost::split(split_config, config, boost::is_any_of(":"));
                assert(split_config.size() == 2);

                int senderCandidates = atoi(split_config[0].c_str());
                int receiverCandidates = atoi(split_config[1].c_str());

                if (!runTest(senderCandidates, receiverCandidates)) {
                    failedTests++;
                    failedTestPairs.push_back(std::pair<int, int>(senderCandidates, receiverCandidates));
                }
            }

            for (auto&& failedTest : failedTestPairs) {
                std::cout << "Failed test: " << "( " << failedTest.first << ", " << failedTest.second << ") " << std::endl;
            }
        }
        else {
            // test all configurations
            for (int n = 0; n < (1 << 7); n++) {
                int senderCandidates = n & 0xF;
                int receiverCandidates = (n >> 4) & 0xF;
                std::cout << "Run test " << n + 1 << " of " << (1 << 7) << ", (" << senderCandidates << ", " << receiverCandidates << ")" << std::endl;
                if (!runTest(senderCandidates, receiverCandidates)) {
                    failedTests++;
                    failedTestPairs.push_back(std::pair<int, int>(senderCandidates, receiverCandidates));
                }
            }

            for (auto&& failedTest : failedTestPairs) {
                std::cout << "Failed test: " << "( " << failedTest.first << ", " << failedTest.second << ") " << std::endl;
            }
        }
    }
    else if (argc == 3) {
        Log::setLogLevel(Log::debug);
        int senderCandidates = atoi(argv[1]);
        int receiverCandidates = atoi(argv[2]);
        if (!runTest(senderCandidates, receiverCandidates)) {
            failedTests++;
        }
    }
    else {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t- to test all combinations pass no arguments" << std::endl;
        std::cout << "\t- to test a specific combination pass two integers describing sender and receiver candidates" << std::endl;
    }
    return failedTests;
}
