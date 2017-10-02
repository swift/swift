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
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>

using namespace Swift;

class EventLoopTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(EventLoopTest);
        CPPUNIT_TEST(testPost);
        CPPUNIT_TEST(testRemove);
        CPPUNIT_TEST(testHandleEvent_Recursive);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            events_.clear();
        }

        void testPost() {
            SimpleEventLoop testling;

            testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 1));
            testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 2));
            testling.stop();
            testling.run();

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(events_.size()));
            CPPUNIT_ASSERT_EQUAL(1, events_[0]);
            CPPUNIT_ASSERT_EQUAL(2, events_[1]);
        }

        void testRemove() {
            SimpleEventLoop testling;
            std::shared_ptr<MyEventOwner> eventOwner1(new MyEventOwner());
            std::shared_ptr<MyEventOwner> eventOwner2(new MyEventOwner());

            testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 1), eventOwner1);
            testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 2), eventOwner2);
            testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 3), eventOwner1);
            testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 4), eventOwner2);
            testling.removeEventsFromOwner(eventOwner2);
            testling.stop();
            testling.run();

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(events_.size()));
            CPPUNIT_ASSERT_EQUAL(1, events_[0]);
            CPPUNIT_ASSERT_EQUAL(3, events_[1]);
        }

        void testHandleEvent_Recursive() {
            DummyEventLoop testling;
            std::shared_ptr<MyEventOwner> eventOwner(new MyEventOwner());

            testling.postEvent(boost::bind(&EventLoopTest::runEventLoop, this, &testling, eventOwner), eventOwner);
            testling.postEvent(boost::bind(&EventLoopTest::logEvent, this, 0), eventOwner);
            testling.processEvents();

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(events_.size()));
            CPPUNIT_ASSERT_EQUAL(0, events_[0]);
            CPPUNIT_ASSERT_EQUAL(1, events_[1]);
        }

    private:
        struct MyEventOwner : public EventOwner {};
        void logEvent(int i) {
            events_.push_back(i);
        }
        void runEventLoop(DummyEventLoop* loop, std::shared_ptr<MyEventOwner> eventOwner) {
            loop->processEvents();
            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(events_.size()));
            loop->postEvent(boost::bind(&EventLoopTest::logEvent, this, 1), eventOwner);
        }

    private:
        std::vector<int> events_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(EventLoopTest);
