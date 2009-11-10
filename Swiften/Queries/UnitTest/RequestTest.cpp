#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "Swiften/Queries/GenericRequest.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Queries/DummyIQChannel.h"
#include "Swiften/Elements/Payload.h"

using namespace Swift;

class RequestTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(RequestTest);
		CPPUNIT_TEST(testSendGet);
		CPPUNIT_TEST(testSendSet);
		CPPUNIT_TEST(testHandleIQ);
		CPPUNIT_TEST(testHandleIQ_InvalidID);
		CPPUNIT_TEST(testHandleIQ_Error);
		CPPUNIT_TEST(testHandleIQ_BeforeSend);
		CPPUNIT_TEST_SUITE_END();

	public:
		class MyPayload : public Payload {
			public:
				MyPayload(const String& s = "") : text_(s) {}
				String text_;
		};

		typedef GenericRequest<MyPayload> MyRequest;

	public:
		RequestTest() {}

		void setUp() {
			channel_ = new DummyIQChannel();
			router_ = new IQRouter(channel_);
			payload_ = boost::shared_ptr<Payload>(new MyPayload("foo"));
			responsePayload_ = boost::shared_ptr<Payload>(new MyPayload("bar"));
			responsesReceived_ = 0;
			errorsReceived_ = 0;
		}

		void tearDown() {
			delete router_;
			delete channel_;
		}

		void testSendSet() {
			MyRequest testling(IQ::Set, JID("foo@bar.com/baz"), payload_, router_);
			testling.send();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), channel_->iqs_[0]->getTo());
			CPPUNIT_ASSERT_EQUAL(IQ::Set, channel_->iqs_[0]->getType());
			CPPUNIT_ASSERT_EQUAL(String("test-id"), channel_->iqs_[0]->getID());
		}

		void testSendGet() {
			MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
			testling.send();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
			CPPUNIT_ASSERT_EQUAL(IQ::Get, channel_->iqs_[0]->getType());
		}

		void testHandleIQ() {
			MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
			testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
			testling.send();
			
			channel_->onIQReceived(createResponse("test-id"));

			CPPUNIT_ASSERT_EQUAL(1, responsesReceived_);
			CPPUNIT_ASSERT_EQUAL(0, errorsReceived_);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
		}

		// FIXME: Doesn't test that it didn't handle the payload
		void testHandleIQ_InvalidID() {
			MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
			testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
			testling.send();

			channel_->onIQReceived(createResponse("different-id"));

			CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
			CPPUNIT_ASSERT_EQUAL(0, errorsReceived_);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
		}

		void testHandleIQ_Error() {
			MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
			testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
			testling.send();

			channel_->onIQReceived(createError("test-id"));

			CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
			CPPUNIT_ASSERT_EQUAL(1, errorsReceived_);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel_->iqs_.size()));
		}

		void testHandleIQ_BeforeSend() {
			MyRequest testling(IQ::Get, JID("foo@bar.com/baz"), payload_, router_);
			testling.onResponse.connect(boost::bind(&RequestTest::handleResponse, this, _1, _2));
			channel_->onIQReceived(createResponse("test-id"));

			CPPUNIT_ASSERT_EQUAL(0, responsesReceived_);
			CPPUNIT_ASSERT_EQUAL(0, errorsReceived_);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(channel_->iqs_.size()));
		}
	
	private:
		void handleResponse(boost::shared_ptr<Payload> p, const boost::optional<ErrorPayload>& e) {
			if (e) {
				++errorsReceived_;
			}
			else {
				boost::shared_ptr<MyPayload> payload(boost::dynamic_pointer_cast<MyPayload>(p));
				CPPUNIT_ASSERT(payload);
				CPPUNIT_ASSERT_EQUAL(String("bar"), payload->text_);
				++responsesReceived_;
			}
		}

		boost::shared_ptr<IQ> createResponse(const String& id) {
			boost::shared_ptr<IQ> iq(new IQ(IQ::Result));
			iq->addPayload(responsePayload_);
			iq->setID(id);
			return iq;
		}

		boost::shared_ptr<IQ> createError(const String& id) {
			boost::shared_ptr<IQ> iq(new IQ(IQ::Error));
			iq->setID(id);
			return iq;
		}
	
	private:
		IQRouter* router_;
		DummyIQChannel* channel_;
		boost::shared_ptr<Payload> payload_;
		boost::shared_ptr<Payload> responsePayload_;
		int responsesReceived_;
		int errorsReceived_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(RequestTest);
