/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/FileTransfer/OutgoingJingleFileTransfer.h>
#include <Swiften/Jingle/FakeJingleSession.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelectorFactory.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGeneratorFactory.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/FileTransfer/ByteArrayReadBytestream.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxy.h>

#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/IBB.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/Network/PlatformNATTraversalWorker.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Network/DummyConnection.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/DummyConnectionFactory.h>

#include <Swiften/Base/Log.h>

#include <iostream>

using namespace Swift;

class OFakeRemoteJingleTransportCandidateSelector : public RemoteJingleTransportCandidateSelector {
		void addRemoteTransportCandidates(JingleTransportPayload::ref cand) {
			candidate = cand;
		}

		void selectCandidate() {
			JingleS5BTransportPayload::ref payload = boost::make_shared<JingleS5BTransportPayload>();
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

class OFakeRemoteJingleTransportCandidateSelectorFactory : public RemoteJingleTransportCandidateSelectorFactory {
public:
	virtual ~OFakeRemoteJingleTransportCandidateSelectorFactory() {

	}

	virtual RemoteJingleTransportCandidateSelector* createCandidateSelector() {
		return new OFakeRemoteJingleTransportCandidateSelector();
	}
};

class OFakeLocalJingleTransportCandidateGenerator : public LocalJingleTransportCandidateGenerator {
public:
	virtual void generateLocalTransportCandidates(JingleTransportPayload::ref /* payload */) {
		//JingleTransportPayload::ref payL = make_shared<JingleTransportPayload>();
		//payL->setSessionID(payload->getSessionID());
		JingleS5BTransportPayload::ref payL = boost::make_shared<JingleS5BTransportPayload>();

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

class OFakeLocalJingleTransportCandidateGeneratorFactory : public LocalJingleTransportCandidateGeneratorFactory {
public:
	virtual LocalJingleTransportCandidateGenerator* createCandidateGenerator() {
		return new OFakeLocalJingleTransportCandidateGenerator();
	}
};

class OutgoingJingleFileTransferTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(OutgoingJingleFileTransferTest);
		CPPUNIT_TEST(test_SendSessionInitiateOnStart);
		CPPUNIT_TEST(test_IBBStartsAfterSendingSessionAccept);
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
			StreamInitiationFileInfo fileInfo;
			fileInfo.setDescription("some file");
			fileInfo.setName("test.bin");
			fileInfo.setHash("asdjasdas");
			fileInfo.setSize(1024 * 1024);
			return boost::shared_ptr<OutgoingJingleFileTransfer>(new OutgoingJingleFileTransfer(boost::shared_ptr<JingleSession>(fakeJingleSession), fakeRJTCSF.get(), fakeLJTCF.get(), iqRouter, idGen, JID(), to, stream, fileInfo, s5bRegistry, s5bProxy));
		}

		IQ::ref createIBBRequest(IBB::ref ibb, const JID& from, const std::string& id) {
			IQ::ref request = IQ::createRequest(IQ::Set, JID("foo@bar.com/baz"), id, ibb);
			request->setFrom(from);
			return request;
		}

		void setUp() {
			fakeJingleSession = new FakeJingleSession("foo@bar.com/baz", "mysession");
			jingleContentPayload = boost::make_shared<JingleContentPayload>();
			fakeRJTCSF = boost::make_shared<OFakeRemoteJingleTransportCandidateSelectorFactory>();
			fakeLJTCF = boost::make_shared<OFakeLocalJingleTransportCandidateGeneratorFactory>();
			stanzaChannel = new DummyStanzaChannel();
			iqRouter = new IQRouter(stanzaChannel);
			eventLoop = new DummyEventLoop();
			timerFactory = new DummyTimerFactory();
			connectionFactory = new DummyConnectionFactory(eventLoop);
			s5bRegistry = new SOCKS5BytestreamRegistry();
			s5bProxy = new SOCKS5BytestreamProxy(connectionFactory, timerFactory);

			data.clear();
			for (int n=0; n < 1024 * 1024; ++n) {
				data.push_back(34);
			}
			
			stream = boost::make_shared<ByteArrayReadBytestream>(data);

			idGen = new IDGenerator();
		}

		void tearDown() {
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
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), description->getOffers().size());
			CPPUNIT_ASSERT(static_cast<size_t>(1048576) == description->getOffers()[0].getSize());

			JingleS5BTransportPayload::ref transport = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(call.payload);
			CPPUNIT_ASSERT(transport);
		}
		
		void test_IBBStartsAfterSendingSessionAccept() {
			boost::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling();
			transfer->start();

			FakeJingleSession::InitiateCall call = getCall<FakeJingleSession::InitiateCall>(0);
			// FIXME: we initiate with SOCSK5 now and not IBB, needs to be fixed.
			/*
			fakeJingleSession->onSessionAcceptReceived(call.id, call.description, call.payload);
			IQ::ref iqOpenStanza = stanzaChannel->getStanzaAtIndex<IQ>(0);
			CPPUNIT_ASSERT(iqOpenStanza);
			*/
		}
		
		void test_ReceiveSessionTerminateAfterSessionInitiate() {
			boost::shared_ptr<OutgoingJingleFileTransfer> transfer = createTestling();
			transfer->start();
			
			getCall<FakeJingleSession::InitiateCall>(0);

			FTStatusHelper helper;
			helper.finishedCalled = false;
			transfer->onFinished.connect(bind(&FTStatusHelper::handleFileTransferFinished, &helper, _1));
			fakeJingleSession->onSessionTerminateReceived(JinglePayload::Reason(JinglePayload::Reason::Busy));
			CPPUNIT_ASSERT_EQUAL(true, helper.finishedCalled);
			CPPUNIT_ASSERT(FileTransferError::PeerError == helper.error);
		}
		

//TODO: some more testcases

private:
	void addFileTransferDescription() {
		boost::shared_ptr<JingleFileTransferDescription> desc = boost::make_shared<JingleFileTransferDescription>();
		desc->addOffer(StreamInitiationFileInfo());
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
	boost::shared_ptr<OFakeRemoteJingleTransportCandidateSelectorFactory> fakeRJTCSF;
	boost::shared_ptr<OFakeLocalJingleTransportCandidateGeneratorFactory> fakeLJTCF;
	DummyStanzaChannel* stanzaChannel;
	IQRouter* iqRouter;
	IDGenerator* idGen;
	EventLoop *eventLoop;
	SOCKS5BytestreamRegistry* s5bRegistry;
	SOCKS5BytestreamProxy* s5bProxy;
	DummyTimerFactory* timerFactory;
	DummyConnectionFactory* connectionFactory;
};

CPPUNIT_TEST_SUITE_REGISTRATION(OutgoingJingleFileTransferTest);
