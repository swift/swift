/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Base/Override.h>
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

#include <iostream>

using namespace Swift;

class OutgoingJingleFileTransferTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(OutgoingJingleFileTransferTest);
		CPPUNIT_TEST(test_SendSessionInitiateOnStart);
		CPPUNIT_TEST(test_FallbackToIBBAfterFailingS5B);
		CPPUNIT_TEST(test_ReceiveSessionTerminateAfterSessionInitiate);
		CPPUNIT_TEST_SUITE_END();

		class FTStatusHelper {
		public:
			bool finishedCalled;
			FileTransferError::Type error;
			void handleFileTransferFinished(boost::optional<FileTransferError> error) {
				finishedCalled = true;
				if (error.is_initialized()) this->error = error.get().getType();
			}
		};
public:

		boost::shared_ptr<OutgoingJingleFileTransfer> createTestling() {
			JID to("test@foo.com/bla");
			JingleFileTransferFileInfo fileInfo;
			fileInfo.setDescription("some file");
			fileInfo.setName("test.bin");
			fileInfo.addHash(HashElement("sha-1", ByteArray()));
			fileInfo.setSize(1024 * 1024);
			return boost::shared_ptr<OutgoingJingleFileTransfer>(new OutgoingJingleFileTransfer(
				to,
				boost::shared_ptr<JingleSession>(fakeJingleSession),
				stream,
				ftTransportFactory,
				timerFactory,
				idGen,
				fileInfo,
				FileTransferOptions().withAssistedAllowed(false).withDirectAllowed(false).withProxiedAllowed(false),
				crypto.get()));
		}

		IQ::ref createIBBRequest(IBB::ref ibb, const JID& from, const std::string& id) {
			IQ::ref request = IQ::createRequest(IQ::Set, JID("foo@bar.com/baz"), id, ibb);
			request->setFrom(from);
			return request;
		}

		void setUp() {
			crypto = boost::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
			fakeJingleSession = new FakeJingleSession("foo@bar.com/baz", "mysession");
			jingleContentPayload = boost::make_shared<JingleContentPayload>();
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
			
			stream = boost::make_shared<ByteArrayReadBytestream>(data);

			idGen = new IDGenerator();
			s5bProxy = new SOCKS5BytestreamProxiesManager(connectionFactory, timerFactory, resolver, iqRouter, "bar.com");
			ftTransportFactory = new DummyFileTransferTransporterFactory(s5bRegistry, bytestreamServerManager, s5bProxy, idGen, connectionFactory, timerFactory, crypto.get(), iqRouter);
		}

		void tearDown() {
			delete ftTransportFactory;
			delete bytestreamServerManager;
			delete s5bProxy;
			delete idGen;
			delete s5bRegistry;
			delete connectionFactory;
			delete timerFactory;
			delete eventLoop;
			delete iqRouter;
			delete stanzaChannel;
		}

		
		void test_SendSessionInitiateOnStart() {
			boost::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling();
			transfer->start();
			
			FakeJingleSession::InitiateCall call = getCall<FakeJingleSession::InitiateCall>(0);
			JingleFileTransferDescription::ref description = boost::dynamic_pointer_cast<JingleFileTransferDescription>(call.description);
			CPPUNIT_ASSERT(description);
			CPPUNIT_ASSERT(static_cast<size_t>(1048576) == description->getFileInfo().getSize());

			JingleS5BTransportPayload::ref transport = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(call.payload);
			CPPUNIT_ASSERT(transport);
		}
		
		void test_FallbackToIBBAfterFailingS5B() {
			boost::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling();
			transfer->start();

			FakeJingleSession::InitiateCall call = getCall<FakeJingleSession::InitiateCall>(0);

			fakeJingleSession->handleSessionAcceptReceived(call.id, call.description, call.payload);

			// send candidate failure
			JingleS5BTransportPayload::ref candidateFailurePayload = boost::make_shared<JingleS5BTransportPayload>();
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
			boost::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling();
			transfer->start();
			
			getCall<FakeJingleSession::InitiateCall>(0);

			FTStatusHelper helper;
			helper.finishedCalled = false;
			transfer->onFinished.connect(bind(&FTStatusHelper::handleFileTransferFinished, &helper, _1));
			fakeJingleSession->handleSessionTerminateReceived(JinglePayload::Reason(JinglePayload::Reason::Busy));
			CPPUNIT_ASSERT_EQUAL(true, helper.finishedCalled);
			CPPUNIT_ASSERT(FileTransferError::PeerError == helper.error);
		}
		

//TODO: some more testcases

private:
	void addFileTransferDescription() {
		boost::shared_ptr<JingleFileTransferDescription> desc = boost::make_shared<JingleFileTransferDescription>();
		desc->setFileInfo(JingleFileTransferFileInfo());
		jingleContentPayload->addDescription(desc);
	}

	boost::shared_ptr<JingleS5BTransportPayload> addJingleS5BPayload() {
		JingleS5BTransportPayload::ref payLoad = boost::make_shared<JingleS5BTransportPayload>();
		payLoad->setSessionID("mysession");
		jingleContentPayload->addTransport(payLoad);
		return payLoad;
	}

	boost::shared_ptr<JingleIBBTransportPayload> addJingleIBBPayload() {
		JingleIBBTransportPayload::ref payLoad = boost::make_shared<JingleIBBTransportPayload>();
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
	boost::shared_ptr<ByteArrayReadBytestream> stream;
	FakeJingleSession* fakeJingleSession;
	boost::shared_ptr<JingleContentPayload> jingleContentPayload;
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
	boost::shared_ptr<CryptoProvider> crypto;
	NetworkEnvironment* networkEnvironment;
	NATTraverser* natTraverser;
	DomainNameResolver* resolver;
};

CPPUNIT_TEST_SUITE_REGISTRATION(OutgoingJingleFileTransferTest);
