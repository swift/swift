/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Base/ByteArray.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>
#include <boost/bind.hpp>

#include "Swiften/FileTransfer/IBBSendSession.h"
#include "Swiften/FileTransfer/ByteArrayReadBytestream.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Client/DummyStanzaChannel.h"

using namespace Swift;

class IBBSendSessionTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(IBBSendSessionTest);
		CPPUNIT_TEST(testStart);
		CPPUNIT_TEST(testStart_ResponseStartsSending);
		CPPUNIT_TEST(testResponseContinuesSending);
		CPPUNIT_TEST(testRespondToAllFinishes);
		CPPUNIT_TEST(testErrorResponseFinishesWithError);
		CPPUNIT_TEST(testStopDuringSessionCloses);
		CPPUNIT_TEST(testStopAfterFinishedDoesNotClose);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			stanzaChannel = new DummyStanzaChannel();
			iqRouter = new IQRouter(stanzaChannel);
			bytestream = boost::shared_ptr<ByteArrayReadBytestream>(new ByteArrayReadBytestream(ByteArray("abcdefg")));
		}

		void tearDown() {
			delete iqRouter;
			delete stanzaChannel;
		}

		void testStart() {
			std::auto_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(1234);

			testling->start();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<IBB>(0, JID("foo@bar.com/baz"), IQ::Set));
			IBB::ref ibb = stanzaChannel->sentStanzas[0]->getPayload<IBB>();
			CPPUNIT_ASSERT_EQUAL(IBB::Open, ibb->getAction());
			CPPUNIT_ASSERT_EQUAL(1234, ibb->getBlockSize());
			CPPUNIT_ASSERT_EQUAL(std::string("myid"), ibb->getStreamID());
		}

		void testStart_ResponseStartsSending() {
			std::auto_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(3);
			testling->start();

			stanzaChannel->onIQReceived(createIBBResult());

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<IBB>(1, JID("foo@bar.com/baz"), IQ::Set));
			IBB::ref ibb = stanzaChannel->sentStanzas[1]->getPayload<IBB>();
			CPPUNIT_ASSERT_EQUAL(IBB::Data, ibb->getAction());
			CPPUNIT_ASSERT_EQUAL(ByteArray("abc"), ibb->getData());
			CPPUNIT_ASSERT_EQUAL(0, ibb->getSequenceNumber());
			CPPUNIT_ASSERT_EQUAL(std::string("myid"), ibb->getStreamID());
		}

		void testResponseContinuesSending() {
			std::auto_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());

			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<IBB>(2, JID("foo@bar.com/baz"), IQ::Set));
			IBB::ref ibb = stanzaChannel->sentStanzas[2]->getPayload<IBB>();
			CPPUNIT_ASSERT_EQUAL(IBB::Data, ibb->getAction());
			CPPUNIT_ASSERT_EQUAL(ByteArray("def"), ibb->getData());
			CPPUNIT_ASSERT_EQUAL(1, ibb->getSequenceNumber());
			CPPUNIT_ASSERT_EQUAL(std::string("myid"), ibb->getStreamID());
		}

		void testRespondToAllFinishes() {
			std::auto_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());

			CPPUNIT_ASSERT(finished);
			CPPUNIT_ASSERT(!error);
		}

		void testErrorResponseFinishesWithError() {
			std::auto_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

			CPPUNIT_ASSERT(finished);
			CPPUNIT_ASSERT(error);
		}

		void testStopDuringSessionCloses() {
			std::auto_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(3);
			testling->start();
			testling->stop();

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<IBB>(1, JID("foo@bar.com/baz"), IQ::Set));
			IBB::ref ibb = stanzaChannel->sentStanzas[1]->getPayload<IBB>();
			CPPUNIT_ASSERT_EQUAL(IBB::Close, ibb->getAction());
			CPPUNIT_ASSERT_EQUAL(std::string("myid"), ibb->getStreamID());
			CPPUNIT_ASSERT(finished);
			CPPUNIT_ASSERT(!error);
		}

		void testStopAfterFinishedDoesNotClose() {
			std::auto_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(16);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			CPPUNIT_ASSERT(finished);

			testling->stop();

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}
	
	private:
		IQ::ref createIBBResult() {
			return IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[stanzaChannel->sentStanzas.size()-1]->getID(), boost::shared_ptr<IBB>());
		}

	private:
		std::auto_ptr<IBBSendSession> createSession(const std::string& to) {
			std::auto_ptr<IBBSendSession> session(new IBBSendSession("myid", JID(to), bytestream, iqRouter));
			session->onFinished.connect(boost::bind(&IBBSendSessionTest::handleFinished, this, _1));
			return session;
		}

		void handleFinished(boost::optional<FileTransferError> error) {
			finished = true;
			this->error = error;
		}

	private:
		DummyStanzaChannel* stanzaChannel;
		IQRouter* iqRouter;
		bool finished;
		boost::optional<FileTransferError> error;
		boost::shared_ptr<ByteArrayReadBytestream> bytestream;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IBBSendSessionTest);
