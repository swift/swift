/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/ByteArrayWriteBytestream.h>
#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGeneratorFactory.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelectorFactory.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxy.h>
#include <Swiften/Jingle/FakeJingleSession.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/DummyConnectionFactory.h>
#include <Swiften/Network/PlatformNATTraversalWorker.h>
#include <Swiften/Queries/IQRouter.h>

#include <iostream>

using namespace Swift;
using namespace boost;

class FakeRemoteJingleTransportCandidateSelector : public RemoteJingleTransportCandidateSelector {
		void addRemoteTransportCandidates(JingleTransportPayload::ref cand) {
			candidate = cand;
		}

		void selectCandidate() {
		    boost::shared_ptr<JingleS5BTransportPayload> payload = make_shared<JingleS5BTransportPayload>();
		    payload->setCandidateError(true);
		    payload->setSessionID(candidate->getSessionID());
		    onRemoteTransportCandidateSelectFinished(payload);
		}

		void setMinimumPriority(int) {

		}

		bool isActualCandidate(JingleTransportPayload::ref) {
			return false;
		}

		int getPriority(JingleTransportPayload::ref) {
			return 0;
		}

		JingleTransport::ref selectTransport(JingleTransportPayload::ref) {
			return JingleTransport::ref();
		}

private:
	JingleTransportPayload::ref candidate;
};

class FakeRemoteJingleTransportCandidateSelectorFactory : public RemoteJingleTransportCandidateSelectorFactory {
public:
	virtual ~FakeRemoteJingleTransportCandidateSelectorFactory() {

	}

	virtual RemoteJingleTransportCandidateSelector* createCandidateSelector() {
		return new FakeRemoteJingleTransportCandidateSelector();
	}
};

class FakeLocalJingleTransportCandidateGenerator : public LocalJingleTransportCandidateGenerator {
public:
	virtual void generateLocalTransportCandidates(JingleTransportPayload::ref payload) {
		JingleS5BTransportPayload::ref payL = make_shared<JingleS5BTransportPayload>();
		payL->setSessionID(payload->getSessionID());
		onLocalTransportCandidatesGenerated(payL);
	}

	void emitonLocalTransportCandidatesGenerated(JingleTransportPayload::ref payload) {
		onLocalTransportCandidatesGenerated(payload);
	}

	virtual bool isActualCandidate(JingleTransportPayload::ref) {
		return false;
	}

	virtual int getPriority(JingleTransportPayload::ref) {
		return 0;
	}

	virtual JingleTransport::ref selectTransport(JingleTransportPayload::ref) {
		return JingleTransport::ref();
	}
};

class FakeLocalJingleTransportCandidateGeneratorFactory : public LocalJingleTransportCandidateGeneratorFactory {
public:
	virtual LocalJingleTransportCandidateGenerator* createCandidateGenerator() {
		return new FakeLocalJingleTransportCandidateGenerator();
	}
};

class IncomingJingleFileTransferTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(IncomingJingleFileTransferTest);
		CPPUNIT_TEST(test_AcceptOnyIBBSendsSessionAccept);
		CPPUNIT_TEST(test_OnlyIBBTransferReceiveWorks);
		CPPUNIT_TEST(test_AcceptFailingS5BFallsBackToIBB);
		CPPUNIT_TEST_SUITE_END();
public:
		shared_ptr<IncomingJingleFileTransfer> createTestling() {
			JID ourJID("our@jid.org/full");
			return make_shared<IncomingJingleFileTransfer>(ourJID, shared_ptr<JingleSession>(fakeJingleSession), jingleContentPayload, fakeRJTCSF.get(), fakeLJTCF.get(), iqRouter, bytestreamRegistry, bytestreamProxy, timerFactory);
		}

		IQ::ref createIBBRequest(IBB::ref ibb, const JID& from, const std::string& id) {
			IQ::ref request = IQ::createRequest(IQ::Set, JID("foo@bar.com/baz"), id, ibb);
			request->setFrom(from);
			return request;
		}

		void setUp() {
			eventLoop = new DummyEventLoop();
			fakeJingleSession = new FakeJingleSession("foo@bar.com/baz", "mysession");
			jingleContentPayload = make_shared<JingleContentPayload>();
			fakeRJTCSF = make_shared<FakeRemoteJingleTransportCandidateSelectorFactory>();
			fakeLJTCF = make_shared<FakeLocalJingleTransportCandidateGeneratorFactory>();
			stanzaChannel = new DummyStanzaChannel();
			iqRouter = new IQRouter(stanzaChannel);
			bytestreamRegistry = new SOCKS5BytestreamRegistry();
			timerFactory = new DummyTimerFactory();
			connectionFactory = new DummyConnectionFactory(eventLoop);
			bytestreamProxy = new SOCKS5BytestreamProxy(connectionFactory, timerFactory);
		}

		void tearDown() {
			delete bytestreamProxy;
			delete connectionFactory;
			delete timerFactory;
			delete bytestreamRegistry;
			delete iqRouter;
			delete stanzaChannel;
			delete eventLoop;
		}

		// Tests whether IncomingJingleFileTransfer would accept a IBB only file transfer.
		void test_AcceptOnyIBBSendsSessionAccept() {
			//1. create your test incoming file transfer
			shared_ptr<JingleFileTransferDescription> desc = make_shared<JingleFileTransferDescription>();
			desc->addOffer(StreamInitiationFileInfo("foo.txt", "", 10));
			jingleContentPayload->addDescription(desc);
			JingleIBBTransportPayload::ref tpRef = make_shared<JingleIBBTransportPayload>();
			tpRef->setSessionID("mysession");
			jingleContentPayload->addTransport(tpRef);

			shared_ptr<IncomingJingleFileTransfer> fileTransfer = createTestling();

			//2. do 'accept' on a dummy writebytestream (you'll have to look if there already is one)
			shared_ptr<ByteArrayWriteBytestream> byteStream = make_shared<ByteArrayWriteBytestream>();
			fileTransfer->accept(byteStream);

			// check whether accept has been called
			getCall<FakeJingleSession::AcceptCall>(0);
		}

		void test_OnlyIBBTransferReceiveWorks() {
			//1. create your test incoming file transfer
			shared_ptr<JingleFileTransferDescription> desc = make_shared<JingleFileTransferDescription>();
			desc->addOffer(StreamInitiationFileInfo("file.txt", "", 10));
			jingleContentPayload->addDescription(desc);
			JingleIBBTransportPayload::ref tpRef = make_shared<JingleIBBTransportPayload>();
			tpRef->setSessionID("mysession");
			jingleContentPayload->addTransport(tpRef);

			shared_ptr<IncomingJingleFileTransfer> fileTransfer = createTestling();

			//2. do 'accept' on a dummy writebytestream (you'll have to look if there already is one)
			shared_ptr<ByteArrayWriteBytestream> byteStream = make_shared<ByteArrayWriteBytestream>();
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

			shared_ptr<IncomingJingleFileTransfer> fileTransfer = createTestling();

			//2. do 'accept' on a dummy writebytestream (you'll have to look if there already is one)
			shared_ptr<ByteArrayWriteBytestream> byteStream = make_shared<ByteArrayWriteBytestream>();
			fileTransfer->accept(byteStream);

			// check whether accept has been called
			FakeJingleSession::AcceptCall acceptCall = getCall<FakeJingleSession::AcceptCall>(0);
			CPPUNIT_ASSERT_EQUAL(payLoad->getSessionID(), acceptCall.payload->getSessionID());

			// check for candidate error
			FakeJingleSession::InfoTransportCall infoTransportCall = getCall<FakeJingleSession::InfoTransportCall>(1);
			JingleS5BTransportPayload::ref s5bPayload = dynamic_pointer_cast<JingleS5BTransportPayload>(infoTransportCall.payload);
			CPPUNIT_ASSERT(s5bPayload->hasCandidateError());

			// indicate transport replace (Romeo)
			fakeJingleSession->onTransportReplaceReceived(getContentID(), addJingleIBBPayload());

			FakeJingleSession::AcceptTransportCall acceptTransportCall = getCall<FakeJingleSession::AcceptTransportCall>(2);

			// send a bit of data
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 0, createByteArray("abc")), "foo@bar.com/baz", "id-a"));
			CPPUNIT_ASSERT(createByteArray("abc") == byteStream->getData());
		}

		void test_S5BTransferReceiveTest() {
			addFileTransferDescription();
			JingleS5BTransportPayload::ref payLoad = addJingleS5BPayload();
		}

private:
	void addFileTransferDescription() {
		shared_ptr<JingleFileTransferDescription> desc = make_shared<JingleFileTransferDescription>();
		desc->addOffer(StreamInitiationFileInfo("file.txt", "", 10));
		jingleContentPayload->addDescription(desc);
	}

	shared_ptr<JingleS5BTransportPayload> addJingleS5BPayload() {
		JingleS5BTransportPayload::ref payLoad = make_shared<JingleS5BTransportPayload>();
		payLoad->setSessionID("mysession");
		jingleContentPayload->addTransport(payLoad);
		return payLoad;
	}

	shared_ptr<JingleIBBTransportPayload> addJingleIBBPayload() {
		JingleIBBTransportPayload::ref payLoad = make_shared<JingleIBBTransportPayload>();
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
	EventLoop* eventLoop;
	FakeJingleSession* fakeJingleSession;
	shared_ptr<JingleContentPayload> jingleContentPayload;
	shared_ptr<FakeRemoteJingleTransportCandidateSelectorFactory> fakeRJTCSF;
	shared_ptr<FakeLocalJingleTransportCandidateGeneratorFactory> fakeLJTCF;
	DummyStanzaChannel* stanzaChannel;
	IQRouter* iqRouter;
	SOCKS5BytestreamRegistry* bytestreamRegistry;
	DummyConnectionFactory* connectionFactory;
	SOCKS5BytestreamProxy* bytestreamProxy;
	DummyTimerFactory* timerFactory;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IncomingJingleFileTransferTest);
