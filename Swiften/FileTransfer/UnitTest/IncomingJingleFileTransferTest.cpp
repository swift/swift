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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/FileTransfer/ByteArrayWriteBytestream.h>
#include <Swiften/FileTransfer/DefaultFileTransferTransporterFactory.h>
#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxiesManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>
#include <Swiften/Jingle/FakeJingleSession.h>
#include <Swiften/Network/DummyConnectionFactory.h>
#include <Swiften/Network/DummyConnectionServerFactory.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Network/NATTraverser.h>
#include <Swiften/Network/PlatformNATTraversalWorker.h>
#include <Swiften/Network/PlatformNetworkEnvironment.h>
#include <Swiften/Network/StaticDomainNameResolver.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class IncomingJingleFileTransferTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(IncomingJingleFileTransferTest);
        CPPUNIT_TEST(test_AcceptOnyIBBSendsSessionAccept);
        CPPUNIT_TEST(test_OnlyIBBTransferReceiveWorks);
        //CPPUNIT_TEST(test_AcceptFailingS5BFallsBackToIBB);
        CPPUNIT_TEST_SUITE_END();
public:
        std::shared_ptr<IncomingJingleFileTransfer> createTestling() {
            JID ourJID("our@jid.org/full");
            return std::make_shared<IncomingJingleFileTransfer>(ourJID, std::shared_ptr<JingleSession>(session), jingleContentPayload, ftTransporterFactory, timerFactory, crypto.get());
        }

        IQ::ref createIBBRequest(IBB::ref ibb, const JID& from, const std::string& id) {
            IQ::ref request = IQ::createRequest(IQ::Set, JID("foo@bar.com/baz"), id, ibb);
            request->setFrom(from);
            return request;
        }

        void setUp() {
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
            eventLoop = new DummyEventLoop();
            resolver = new StaticDomainNameResolver(eventLoop);
            session = std::make_shared<FakeJingleSession>("foo@bar.com/baz", "mysession");
            jingleContentPayload = std::make_shared<JingleContentPayload>();
            // fakeRJTCSF = std::make_shared<FakeRemoteJingleTransportCandidateSelectorFactory>();
            // fakeLJTCF = std::make_shared<FakeLocalJingleTransportCandidateGeneratorFactory>();
            stanzaChannel = new DummyStanzaChannel();
            connectionFactory = new DummyConnectionFactory(eventLoop);
            serverConnectionFactory = new DummyConnectionServerFactory(eventLoop);
            iqRouter = new IQRouter(stanzaChannel);
            bytestreamRegistry = new SOCKS5BytestreamRegistry();
            networkEnvironment = new PlatformNetworkEnvironment();
            natTraverser = new PlatformNATTraversalWorker(eventLoop);
            bytestreamServerManager = new SOCKS5BytestreamServerManager(bytestreamRegistry, serverConnectionFactory, networkEnvironment, natTraverser);
            idGenerator = new SimpleIDGenerator();
            timerFactory = new DummyTimerFactory();
            bytestreamProxy = new SOCKS5BytestreamProxiesManager(connectionFactory, timerFactory, resolver, iqRouter, "bar.com");
            ftTransporterFactory = new DefaultFileTransferTransporterFactory(bytestreamRegistry, bytestreamServerManager, bytestreamProxy, idGenerator, connectionFactory, timerFactory, crypto.get(), iqRouter);
        }

        void tearDown() {
            delete ftTransporterFactory;
            delete bytestreamProxy;
            delete timerFactory;
            delete idGenerator;
            delete bytestreamServerManager;
            delete natTraverser;
            delete networkEnvironment;
            delete bytestreamRegistry;
            delete iqRouter;
            delete serverConnectionFactory;
            delete connectionFactory;
            delete stanzaChannel;
            delete resolver;
            delete eventLoop;
            Log::setLogLevel(Log::error);
        }

        // Tests whether IncomingJingleFileTransfer would accept a IBB only file transfer.
        void test_AcceptOnyIBBSendsSessionAccept() {
            //1. create your test incoming file transfer
            std::shared_ptr<JingleFileTransferDescription> desc = std::make_shared<JingleFileTransferDescription>();
            desc->setFileInfo(JingleFileTransferFileInfo("foo.txt", "", 10));
            jingleContentPayload->addDescription(desc);
            JingleIBBTransportPayload::ref tpRef = std::make_shared<JingleIBBTransportPayload>();
            tpRef->setSessionID("mysession");
            jingleContentPayload->addTransport(tpRef);

            std::shared_ptr<IncomingJingleFileTransfer> fileTransfer = createTestling();

            //2. do 'accept' on a dummy writebytestream (you'll have to look if there already is one)
            std::shared_ptr<ByteArrayWriteBytestream> byteStream = std::make_shared<ByteArrayWriteBytestream>();
            fileTransfer->accept(byteStream);

            // check whether accept has been called
            getCall<FakeJingleSession::AcceptCall>(0);
        }

        void test_OnlyIBBTransferReceiveWorks() {
            //1. create your test incoming file transfer
            std::shared_ptr<JingleFileTransferDescription> desc = std::make_shared<JingleFileTransferDescription>();
            desc->setFileInfo(JingleFileTransferFileInfo("file.txt", "", 10));
            jingleContentPayload->addDescription(desc);
            JingleIBBTransportPayload::ref tpRef = std::make_shared<JingleIBBTransportPayload>();
            tpRef->setSessionID("mysession");
            jingleContentPayload->addTransport(tpRef);

            std::shared_ptr<IncomingJingleFileTransfer> fileTransfer = createTestling();

            //2. do 'accept' on a dummy writebytestream (you'll have to look if there already is one)
            std::shared_ptr<ByteArrayWriteBytestream> byteStream = std::make_shared<ByteArrayWriteBytestream>();
            fileTransfer->accept(byteStream);

            // check whether accept has been called
            getCall<FakeJingleSession::AcceptCall>(0);
            stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));
            stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 0, createByteArray("abc")), "foo@bar.com/baz", "id-a"));
            CPPUNIT_ASSERT(createByteArray("abc") == byteStream->getData());
        }

        void test_AcceptFailingS5BFallsBackToIBB() {
            //1. create your test incoming file transfer
            addFileTransferDescription();

            // add SOCKS5BytestreamTransportPayload
            JingleS5BTransportPayload::ref payLoad = addJingleS5BPayload();

            std::shared_ptr<IncomingJingleFileTransfer> fileTransfer = createTestling();

            //2. do 'accept' on a dummy writebytestream (you'll have to look if there already is one)
            std::shared_ptr<ByteArrayWriteBytestream> byteStream = std::make_shared<ByteArrayWriteBytestream>();
            fileTransfer->accept(byteStream);

            // candidates are gathered

            // check whether accept has been called
            FakeJingleSession::AcceptCall acceptCall = getCall<FakeJingleSession::AcceptCall>(0);
            CPPUNIT_ASSERT_EQUAL(payLoad->getSessionID(), acceptCall.payload->getSessionID());

            // check for candidate error
            FakeJingleSession::InfoTransportCall infoTransportCall = getCall<FakeJingleSession::InfoTransportCall>(1);
            JingleS5BTransportPayload::ref s5bPayload = std::dynamic_pointer_cast<JingleS5BTransportPayload>(infoTransportCall.payload);
            CPPUNIT_ASSERT(s5bPayload->hasCandidateError());

            // indicate transport replace (Romeo)
            session->handleTransportReplaceReceived(getContentID(), addJingleIBBPayload());

            FakeJingleSession::AcceptTransportCall acceptTransportCall = getCall<FakeJingleSession::AcceptTransportCall>(2);

            // send a bit of data
            stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));
            stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 0, createByteArray("abc")), "foo@bar.com/baz", "id-a"));
            CPPUNIT_ASSERT(createByteArray("abc") == byteStream->getData());
        }
#if 0
        void test_S5BTransferReceiveTest() {
            addFileTransferDescription();
            JingleS5BTransportPayload::ref payLoad = addJingleS5BPayload();
        }
#endif
private:
    void addFileTransferDescription() {
        std::shared_ptr<JingleFileTransferDescription> desc = std::make_shared<JingleFileTransferDescription>();
        desc->setFileInfo(JingleFileTransferFileInfo("file.txt", "", 10));
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
        CPPUNIT_ASSERT(index < session->calledCommands.size());
        T* cmd = boost::get<T>(&session->calledCommands[index]);
        CPPUNIT_ASSERT(cmd);
        return *cmd;
    }

private:
    EventLoop* eventLoop;
    std::shared_ptr<CryptoProvider> crypto;
    std::shared_ptr<FakeJingleSession> session;
    std::shared_ptr<JingleContentPayload> jingleContentPayload;
//    shared_ptr<FakeRemoteJingleTransportCandidateSelectorFactory> fakeRJTCSF;
//    shared_ptr<FakeLocalJingleTransportCandidateGeneratorFactory> fakeLJTCF;
    FileTransferTransporterFactory* ftTransporterFactory;
    SOCKS5BytestreamServerManager* bytestreamServerManager;
    DummyStanzaChannel* stanzaChannel;
    IQRouter* iqRouter;
    SOCKS5BytestreamRegistry* bytestreamRegistry;
    DummyConnectionFactory* connectionFactory;
    DummyConnectionServerFactory* serverConnectionFactory;
    SOCKS5BytestreamProxiesManager* bytestreamProxy;
    DummyTimerFactory* timerFactory;
    NetworkEnvironment* networkEnvironment;
    NATTraverser* natTraverser;
    IDGenerator* idGenerator;
    DomainNameResolver* resolver;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IncomingJingleFileTransferTest);
