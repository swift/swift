/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>
#include <boost/bind.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/FileTransfer/IBBSendSession.h>
#include <Swiften/FileTransfer/ByteArrayReadBytestream.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Client/DummyStanzaChannel.h>

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
		CPPUNIT_TEST(testDataStreamPauseStopsSendingData);
		CPPUNIT_TEST(testDataStreamResumeAfterPauseSendsData);
		CPPUNIT_TEST(testDataStreamResumeBeforePauseDoesNotSendData);
		CPPUNIT_TEST(testDataStreamResumeAfterResumeDoesNotSendData);

		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			stanzaChannel = new DummyStanzaChannel();
			iqRouter = new IQRouter(stanzaChannel);
			bytestream = boost::make_shared<ByteArrayReadBytestream>(createByteArray("abcdefg"));
			finished = false;
		}

		void tearDown() {
			delete iqRouter;
			delete stanzaChannel;
		}

		void testStart() {
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
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
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(3);
			testling->start();

			stanzaChannel->onIQReceived(createIBBResult());

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<IBB>(1, JID("foo@bar.com/baz"), IQ::Set));
			IBB::ref ibb = stanzaChannel->sentStanzas[1]->getPayload<IBB>();
			CPPUNIT_ASSERT_EQUAL(IBB::Data, ibb->getAction());
			CPPUNIT_ASSERT(createByteArray("abc") == ibb->getData());
			CPPUNIT_ASSERT_EQUAL(0, ibb->getSequenceNumber());
			CPPUNIT_ASSERT_EQUAL(std::string("myid"), ibb->getStreamID());
		}

		void testResponseContinuesSending() {
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());

			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<IBB>(2, JID("foo@bar.com/baz"), IQ::Set));
			IBB::ref ibb = stanzaChannel->sentStanzas[2]->getPayload<IBB>();
			CPPUNIT_ASSERT_EQUAL(IBB::Data, ibb->getAction());
			CPPUNIT_ASSERT(createByteArray("def") == ibb->getData());
			CPPUNIT_ASSERT_EQUAL(1, ibb->getSequenceNumber());
			CPPUNIT_ASSERT_EQUAL(std::string("myid"), ibb->getStreamID());
		}

		void testRespondToAllFinishes() {
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
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
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID()));

			CPPUNIT_ASSERT(finished);
			CPPUNIT_ASSERT(error);
		}

		void testStopDuringSessionCloses() {
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
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
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			testling->setBlockSize(16);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			CPPUNIT_ASSERT(finished);

			testling->stop();

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testDataStreamPauseStopsSendingData() {
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			bytestream->setDataComplete(false);
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());

			CPPUNIT_ASSERT(!finished);
			CPPUNIT_ASSERT(!error);

			CPPUNIT_ASSERT_EQUAL(4, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testDataStreamResumeAfterPauseSendsData() {
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			bytestream->setDataComplete(false);
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());

			bytestream->addData(createByteArray("xyz"));

			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testDataStreamResumeBeforePauseDoesNotSendData() {
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			bytestream->setDataComplete(false);
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());

			bytestream->addData(createByteArray("xyz"));

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testDataStreamResumeAfterResumeDoesNotSendData() {
			boost::shared_ptr<IBBSendSession> testling = createSession("foo@bar.com/baz");
			bytestream->setDataComplete(false);
			testling->setBlockSize(3);
			testling->start();
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());
			stanzaChannel->onIQReceived(createIBBResult());

			bytestream->addData(createByteArray("xyz"));
			bytestream->addData(createByteArray("xuv"));

			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

	private:
		IQ::ref createIBBResult() {
			return IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[stanzaChannel->sentStanzas.size()-1]->getTo(), stanzaChannel->sentStanzas[stanzaChannel->sentStanzas.size()-1]->getID(), boost::shared_ptr<IBB>());
		}

	private:
		boost::shared_ptr<IBBSendSession> createSession(const std::string& to) {
			boost::shared_ptr<IBBSendSession> session(new IBBSendSession("myid", JID(), JID(to), bytestream, iqRouter));
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
