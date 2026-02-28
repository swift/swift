/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>
#include <memory>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/FileTransfer/ByteArrayReadBytestream.h>
#include <Swiften/FileTransfer/DefaultFileTransferTransporterFactory.h>
#include <Swiften/FileTransfer/OutgoingJingleFileTransfer.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxiesManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>
#include <Swiften/FileTransfer/UnitTest/DummyFileTransferTransporterFactory.h>
#include <Swiften/Jingle/FakeJingleSession.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DummyConnection.h>
#include <Swiften/Network/DummyConnectionFactory.h>
#include <Swiften/Network/DummyConnectionServer.h>
#include <Swiften/Network/DummyConnectionServerFactory.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Network/PlatformNATTraversalWorker.h>
#include <Swiften/Network/PlatformNetworkEnvironment.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class OutgoingJingleFileTransferTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(OutgoingJingleFileTransferTest);
        CPPUNIT_TEST(test_SendSessionInitiateOnStart);
        CPPUNIT_TEST(test_FallbackToIBBAfterFailingS5B);
        CPPUNIT_TEST(test_ReceiveSessionTerminateAfterSessionInitiate);
        CPPUNIT_TEST(test_DeclineEmitsFinishedStateCanceled);
        CPPUNIT_TEST_SUITE_END();

        class FTStatusHelper {
        public:
            FTStatusHelper() : finishedCalled(false), error(FileTransferError::UnknownError) {
            }

            void handleFileTransferFinished(boost::optional<FileTransferError> error) {
                finishedCalled = true;
                if (error.is_initialized()) this->error = error.get().getType();
            }

            void handleFileTransferStatusChanged(FileTransfer::State fileTransferSTate) {
                state = fileTransferSTate;
            }

        public:
            bool finishedCalled;
            FileTransferError::Type error;
            boost::optional<FileTransfer::State> state;
        };

public:

        std::shared_ptr<OutgoingJingleFileTransfer> createTestling(const FileTransferOptions& options = FileTransferOptions().withAssistedAllowed(false).withDirectAllowed(false).withProxiedAllowed(false)) {
            JID to("test@foo.com/bla");
            JingleFileTransferFileInfo fileInfo;
            fileInfo.setDescription("some file");
            fileInfo.setName("test.bin");
            fileInfo.addHash(HashElement("sha-1", ByteArray()));
            fileInfo.setSize(1024 * 1024);
            return std::shared_ptr<OutgoingJingleFileTransfer>(new OutgoingJingleFileTransfer(
                to,
                std::shared_ptr<JingleSession>(fakeJingleSession),
                stream,
                ftTransportFactory,
                timerFactory,
                idGen,
                fileInfo,
                options,
                crypto.get()));
        }

        IQ::ref createIBBRequest(IBB::ref ibb, const JID& from, const std::string& id) {
            IQ::ref request = IQ::createRequest(IQ::Set, JID("foo@bar.com/baz"), id, ibb);
            request->setFrom(from);
            return request;
        }

        void setUp() {
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
            fakeJingleSession = new FakeJingleSession("foo@bar.com/baz", "mysession");
            jingleContentPayload = std::make_shared<JingleContentPayload>();
            stanzaChannel = new DummyStanzaChannel();
            iqRouter = new IQRouter(stanzaChannel);
            eventLoop = new DummyEventLoop();
            timerFactory = new DummyTimerFactory();
            connectionFactory = new DummyConnectionFactory(eventLoop);
            serverConnectionFactory = new DummyConnectionServerFactory(eventLoop);
            s5bRegistry = new SOCKS5BytestreamRegistry();
            networkEnvironment = new PlatformNetworkEnvironment();
            natTraverser = new PlatformNATTraversalWorker(eventLoop);
            bytestreamServerManager = new SOCKS5BytestreamServerManager(s5bRegistry, serverConnectionFactory, networkEnvironment, natTraverser);

            data.clear();
            for (int n=0; n < 1024 * 1024; ++n) {
                data.push_back(34);
            }

            stream = std::make_shared<ByteArrayReadBytestream>(data);

            idGen = new IDGenerator();
            s5bProxy = new SOCKS5BytestreamProxiesManager(connectionFactory, timerFactory, resolver, iqRouter, "bar.com");
            ftTransportFactory = new DummyFileTransferTransporterFactory(s5bRegistry, bytestreamServerManager, s5bProxy, idGen, connectionFactory, timerFactory, crypto.get(), iqRouter);
        }

        void tearDown() {
            delete ftTransportFactory;
            delete s5bProxy;
            delete idGen;
            delete bytestreamServerManager;
            delete natTraverser;
            delete networkEnvironment;
            delete s5bRegistry;
            delete serverConnectionFactory;
            delete connectionFactory;
            delete timerFactory;
            delete eventLoop;
            delete iqRouter;
            delete stanzaChannel;
        }


        void test_SendSessionInitiateOnStart() {
            std::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling();
            transfer->start();

            FakeJingleSession::InitiateCall call = getCall<FakeJingleSession::InitiateCall>(0);
            JingleFileTransferDescription::ref description = std::dynamic_pointer_cast<JingleFileTransferDescription>(call.description);
            CPPUNIT_ASSERT(description);
            CPPUNIT_ASSERT(static_cast<size_t>(1048576) == description->getFileInfo().getSize());

            JingleIBBTransportPayload::ref transport = std::dynamic_pointer_cast<JingleIBBTransportPayload>(call.payload);
            CPPUNIT_ASSERT(transport);
        }

        void test_FallbackToIBBAfterFailingS5B() {
            std::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling(FileTransferOptions().withAssistedAllowed(true).withDirectAllowed(true).withProxiedAllowed(true));
            transfer->start();

            FakeJingleSession::InitiateCall call = getCall<FakeJingleSession::InitiateCall>(0);

            CPPUNIT_ASSERT(std::dynamic_pointer_cast<JingleS5BTransportPayload>(call.payload));
            fakeJingleSession->handleSessionAcceptReceived(call.id, call.description, call.payload);

            // send candidate failure
            JingleS5BTransportPayload::ref candidateFailurePayload = std::make_shared<JingleS5BTransportPayload>();
            candidateFailurePayload->setCandidateError(true);
            candidateFailurePayload->setSessionID(call.payload->getSessionID());
            fakeJingleSession->handleTransportInfoReceived(call.id, candidateFailurePayload);

            // no S5B candidates -> fallback to IBB
            // call at position 1 is the candidate our candidate error
            FakeJingleSession::ReplaceTransportCall replaceCall = getCall<FakeJingleSession::ReplaceTransportCall>(2);

            // accept transport replace
            fakeJingleSession->handleTransportAcceptReceived(replaceCall.id, replaceCall.payload);

            IQ::ref iqOpenStanza = stanzaChannel->getStanzaAtIndex<IQ>(0);
            CPPUNIT_ASSERT(iqOpenStanza);
            IBB::ref ibbOpen = iqOpenStanza->getPayload<IBB>();
            CPPUNIT_ASSERT(ibbOpen);
            CPPUNIT_ASSERT_EQUAL(IBB::Open, ibbOpen->getAction());
        }

        void test_ReceiveSessionTerminateAfterSessionInitiate() {
            std::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling();
            transfer->start();

            getCall<FakeJingleSession::InitiateCall>(0);

            FTStatusHelper helper;
            helper.finishedCalled = false;
            transfer->onFinished.connect(bind(&FTStatusHelper::handleFileTransferFinished, &helper, _1));
            fakeJingleSession->handleSessionTerminateReceived(JinglePayload::Reason(JinglePayload::Reason::Busy));
            CPPUNIT_ASSERT_EQUAL(true, helper.finishedCalled);
            CPPUNIT_ASSERT(FileTransferError::PeerError == helper.error);
        }

        void test_DeclineEmitsFinishedStateCanceled() {
            std::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling();
            transfer->start();

            getCall<FakeJingleSession::InitiateCall>(0);

            FTStatusHelper helper;
            helper.finishedCalled = false;
            transfer->onFinished.connect(bind(&FTStatusHelper::handleFileTransferFinished, &helper, _1));
            transfer->onStateChanged.connect(bind(&FTStatusHelper::handleFileTransferStatusChanged, &helper, _1));
            fakeJingleSession->handleSessionTerminateReceived(JinglePayload::Reason(JinglePayload::Reason::Decline));
            CPPUNIT_ASSERT_EQUAL(true, helper.finishedCalled);
            CPPUNIT_ASSERT(FileTransferError::UnknownError == helper.error);
            CPPUNIT_ASSERT_EQUAL(true, helper.state.is_initialized());
            CPPUNIT_ASSERT(FileTransfer::State::Canceled == helper.state.get().type);
        }

//TODO: some more testcases

private:
    void addFileTransferDescription() {
        std::shared_ptr<JingleFileTransferDescription> desc = std::make_shared<JingleFileTransferDescription>();
        desc->setFileInfo(JingleFileTransferFileInfo());
        jingleContentPayload->addDescription(desc);
    }

    std::shared_ptr<JingleS5BTransportPayload> addJingleS5BPayload() {
        JingleS5BTransportPayload::ref payLoad = std::make_shared<JingleS5BTransportPayload>();
        payLoad->setSessionID("mysession");
        jingleContentPayload->addTransport(payLoad);
        return payLoad;
    }

    std::shared_ptr<JingleIBBTransportPayload> addJingleIBBPayload() {
        JingleIBBTransportPayload::ref payLoad = std::make_shared<JingleIBBTransportPayload>();
        payLoad->setSessionID("mysession");
        jingleContentPayload->addTransport(payLoad);
        return payLoad;
    }

    JingleContentID getContentID() const  {
        return JingleContentID(jingleContentPayload->getName(), jingleContentPayload->getCreator());
    }

    template <typename T> T getCall(int i) const {
        size_t index = static_cast<size_t>(i);
        CPPUNIT_ASSERT(index < fakeJingleSession->calledCommands.size());
        T* cmd = boost::get<T>(&fakeJingleSession->calledCommands[index]);
        CPPUNIT_ASSERT(cmd);
        return *cmd;
    }

private:
    std::vector<unsigned char> data;
    std::shared_ptr<ByteArrayReadBytestream> stream;
    FakeJingleSession* fakeJingleSession;
    std::shared_ptr<JingleContentPayload> jingleContentPayload;
    FileTransferTransporterFactory* ftTransportFactory;
    SOCKS5BytestreamServerManager* bytestreamServerManager;
    DummyStanzaChannel* stanzaChannel;
    IQRouter* iqRouter;
    IDGenerator* idGen;
    EventLoop *eventLoop;
    SOCKS5BytestreamRegistry* s5bRegistry;
    SOCKS5BytestreamProxiesManager* s5bProxy;
    DummyTimerFactory* timerFactory;
    DummyConnectionFactory* connectionFactory;
    DummyConnectionServerFactory* serverConnectionFactory;
    std::shared_ptr<CryptoProvider> crypto;
    NetworkEnvironment* networkEnvironment;
    NATTraverser* natTraverser;
    DomainNameResolver* resolver;
};

CPPUNIT_TEST_SUITE_REGISTRATION(OutgoingJingleFileTransferTest);
