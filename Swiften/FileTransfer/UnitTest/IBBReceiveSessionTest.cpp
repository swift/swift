/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>
#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/FileTransfer/IBBReceiveSession.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Client/DummyStanzaChannel.h>

using namespace Swift;

class IBBReceiveSessionTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(IBBReceiveSessionTest);
		CPPUNIT_TEST(testOpen);
		CPPUNIT_TEST(testReceiveData);
		CPPUNIT_TEST(testReceiveMultipleData);
		CPPUNIT_TEST(testReceiveDataForOtherSession);
		CPPUNIT_TEST(testReceiveDataOutOfOrder);
		CPPUNIT_TEST(testReceiveLastData);
		CPPUNIT_TEST(testReceiveClose);
		CPPUNIT_TEST(testStopWhileActive);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			stanzaChannel = new DummyStanzaChannel();
			iqRouter = new IQRouter(stanzaChannel);
			finished = false;
		}

		void tearDown() {
			delete iqRouter;
			delete stanzaChannel;
		}

		void testOpen() {
			boost::shared_ptr<IBBReceiveSession> testling(createSession("foo@bar.com/baz", "mysession"));
			testling->start();
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));

			CPPUNIT_ASSERT(stanzaChannel->isResultAtIndex(0, "id-open"));
			CPPUNIT_ASSERT(!finished);

			testling->stop();
		}

		void testReceiveData() {
			boost::shared_ptr<IBBReceiveSession> testling(createSession("foo@bar.com/baz", "mysession"));
			testling->start();
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));

			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 0, createByteArray("abc")), "foo@bar.com/baz", "id-a"));

			CPPUNIT_ASSERT(stanzaChannel->isResultAtIndex(1, "id-a"));
			CPPUNIT_ASSERT(createByteArray("abc") == receivedData);
			CPPUNIT_ASSERT(!finished);

			testling->stop();
		}

		void testReceiveMultipleData() {
			boost::shared_ptr<IBBReceiveSession> testling(createSession("foo@bar.com/baz", "mysession"));
			testling->start();
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));

			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 0, createByteArray("abc")), "foo@bar.com/baz", "id-a"));
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 1, createByteArray("def")), "foo@bar.com/baz", "id-b"));

			CPPUNIT_ASSERT(stanzaChannel->isResultAtIndex(2, "id-b"));
			CPPUNIT_ASSERT(createByteArray("abcdef") == receivedData);
			CPPUNIT_ASSERT(!finished);

			testling->stop();
		}

		void testReceiveDataForOtherSession() {
			boost::shared_ptr<IBBReceiveSession> testling(createSession("foo@bar.com/baz", "mysession"));
			testling->start();
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));

			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("othersession", 0, createByteArray("abc")), "foo@bar.com/baz", "id-a"));

			CPPUNIT_ASSERT(stanzaChannel->isErrorAtIndex(1, "id-a"));

			testling->stop();
		}

		void testReceiveDataOutOfOrder() {
			boost::shared_ptr<IBBReceiveSession> testling(createSession("foo@bar.com/baz", "mysession"));
			testling->start();
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));

			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 0, createByteArray("abc")), "foo@bar.com/baz", "id-a"));
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 0, createByteArray("def")), "foo@bar.com/baz", "id-b"));

			CPPUNIT_ASSERT(stanzaChannel->isErrorAtIndex(2, "id-b"));
			CPPUNIT_ASSERT(finished);
			CPPUNIT_ASSERT(error);

			testling->stop();
		}

		void testReceiveLastData() {
			boost::shared_ptr<IBBReceiveSession> testling(createSession("foo@bar.com/baz", "mysession", 6));
			testling->start();
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));

			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 0, createByteArray("abc")), "foo@bar.com/baz", "id-a"));
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBData("mysession", 1, createByteArray("def")), "foo@bar.com/baz", "id-b"));

			CPPUNIT_ASSERT(stanzaChannel->isResultAtIndex(2, "id-b"));
			CPPUNIT_ASSERT(createByteArray("abcdef") == receivedData);
			CPPUNIT_ASSERT(finished);
			CPPUNIT_ASSERT(!error);

			testling->stop();
		}

		void testReceiveClose() {
			boost::shared_ptr<IBBReceiveSession> testling(createSession("foo@bar.com/baz", "mysession"));
			testling->start();
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));

			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBClose("mysession"), "foo@bar.com/baz", "id-close"));

			CPPUNIT_ASSERT(finished);
			CPPUNIT_ASSERT(error);

			testling->stop();
		}

		void testStopWhileActive() {
			boost::shared_ptr<IBBReceiveSession> testling(createSession("foo@bar.com/baz", "mysession"));
			testling->start();
			stanzaChannel->onIQReceived(createIBBRequest(IBB::createIBBOpen("mysession", 0x10), "foo@bar.com/baz", "id-open"));

			testling->stop();

			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<IBB>(1, JID("foo@bar.com/baz"), IQ::Set));
			IBB::ref ibb = stanzaChannel->sentStanzas[1]->getPayload<IBB>();
			CPPUNIT_ASSERT_EQUAL(IBB::Close, ibb->getAction());
			CPPUNIT_ASSERT_EQUAL(std::string("mysession"), ibb->getStreamID());
			CPPUNIT_ASSERT(finished);
			CPPUNIT_ASSERT(!error);
		}

	private:
		IQ::ref createIBBRequest(IBB::ref ibb, const JID& from, const std::string& id) {
			IQ::ref request = IQ::createRequest(IQ::Set, JID("baz@fum.com/dum"), id, ibb);
			request->setFrom(from);
			return request;
		}

		IBBReceiveSession* createSession(const std::string& from, const std::string& id, size_t size = 0x1000) {
			IBBReceiveSession* session = new IBBReceiveSession(id, JID(from), JID(), size, iqRouter);
			session->onDataReceived.connect(boost::bind(&IBBReceiveSessionTest::handleDataReceived, this, _1));
			session->onFinished.connect(boost::bind(&IBBReceiveSessionTest::handleFinished, this, _1));
			return session;
		}


		void handleFinished(boost::optional<FileTransferError> error) {
			finished = true;
			this->error = error;
		}

		void handleDataReceived(const std::vector<unsigned char>& data) {
			receivedData.insert(receivedData.end(), data.begin(), data.end());
		}

	private:
		DummyStanzaChannel* stanzaChannel;
		IQRouter* iqRouter;
		bool finished;
		boost::optional<FileTransferError> error;
		std::vector<unsigned char> receivedData;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IBBReceiveSessionTest);
