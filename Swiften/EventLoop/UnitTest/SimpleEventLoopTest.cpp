/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Base/sleep.h>

using namespace Swift;

class SimpleEventLoopTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SimpleEventLoopTest);
		// FIXME: Temporarily disabling run, because it generates a "vector 
		// iterator not incrementable" on XP
		//CPPUNIT_TEST(testRun);
		CPPUNIT_TEST(testPostFromMainThread);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			counter_ = 0;
		}

		void testRun() {
			SimpleEventLoop testling;
			boost::thread thread(boost::bind(&SimpleEventLoopTest::runIncrementingThread, this, &testling));
			testling.run();

			CPPUNIT_ASSERT_EQUAL(10, counter_);
		}

		void testPostFromMainThread() {
			SimpleEventLoop testling;
			testling.postEvent(boost::bind(&SimpleEventLoopTest::incrementCounterAndStop, this, &testling));
			testling.run();

			CPPUNIT_ASSERT_EQUAL(1, counter_);
		}

	private:
		void runIncrementingThread(SimpleEventLoop* loop) {
			for (unsigned int i = 0; i < 10; ++i) {
				Swift::sleep(1);
				loop->postEvent(boost::bind(&SimpleEventLoopTest::incrementCounter, this));
			}
			loop->stop();
		}

		void incrementCounter() {
			counter_++;
		}

		void incrementCounterAndStop(SimpleEventLoop* loop) {
			counter_++;
			loop->stop();
		}

		int counter_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SimpleEventLoopTest);
