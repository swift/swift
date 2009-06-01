#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Base/sleep.h"

using namespace Swift;

class EventLoopTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(EventLoopTest);
		CPPUNIT_TEST(testPost);
		CPPUNIT_TEST(testRemove);
		CPPUNIT_TEST_SUITE_END();

	public:
		EventLoopTest() {}

		void setUp() {
			events_.clear();
		}

		void testPost() {
			SimpleEventLoop testling;

			testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 1), 0);
			testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 2), 0);
			testling.stop();
			testling.run();

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(events_.size()));
			CPPUNIT_ASSERT_EQUAL(1, events_[0]);
			CPPUNIT_ASSERT_EQUAL(2, events_[1]);
		}

		void testRemove() {
			SimpleEventLoop testling;

			testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 1), &testling);
			testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 2), this);
			testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 3), &testling);
			testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 4), this);
			testling.removeEventsFromOwner(this);
			testling.stop();
			testling.run();

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(events_.size()));
			CPPUNIT_ASSERT_EQUAL(1, events_[0]);
			CPPUNIT_ASSERT_EQUAL(3, events_[1]);
		}
	
	private:
		void logEvent(int i) {
			events_.push_back(i);
		}

	private:
		std::vector<int> events_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(EventLoopTest);
