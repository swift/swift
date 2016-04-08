/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <thread>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/sleep.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>

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
            std::thread thread(boost::bind(&SimpleEventLoopTest::runIncrementingThread, this, &testling));
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
