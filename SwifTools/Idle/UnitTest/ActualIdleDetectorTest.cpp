/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>

#include <SwifTools/Idle/ActualIdleDetector.h>
#include <SwifTools/Idle/IdleQuerier.h>

using namespace Swift;

class ActualIdleDetectorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ActualIdleDetectorTest);
        CPPUNIT_TEST(testDestructor);
        CPPUNIT_TEST(testHandleTick_Idle);
        CPPUNIT_TEST(testHandleTick_Idle_AlreadyIdle);
        CPPUNIT_TEST(testHandleTick_NotIdle);
        CPPUNIT_TEST(testHandleTick_NotIdle_AlreadyNotIdle);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            querier = std::unique_ptr<MockIdleQuerier>(new MockIdleQuerier());
            timerFactory = std::unique_ptr<MockTimerFactory>(new MockTimerFactory());
            idleEvents.clear();
        }

        void testDestructor()  {
            ActualIdleDetector* testling = createDetector();
            testling->setIdleTimeSeconds(15);
            delete testling;

            querier->idleTime = 15;
            timerFactory->updateTime(15000);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(idleEvents.size()));
        }

        void testHandleTick_Idle() {
            std::unique_ptr<ActualIdleDetector> testling(createDetector());
            testling->setIdleTimeSeconds(15);
            querier->idleTime = 15;

            timerFactory->updateTime(15000);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(idleEvents.size()));
            CPPUNIT_ASSERT(idleEvents[0]);
        }

        void testHandleTick_Idle_AlreadyIdle() {
            std::unique_ptr<ActualIdleDetector> testling(createDetector());
            testling->setIdleTimeSeconds(15);
            querier->idleTime = 15;
            timerFactory->updateTime(15000);

            querier->idleTime = 30;
            timerFactory->updateTime(30000);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(idleEvents.size()));
            CPPUNIT_ASSERT(idleEvents[0]);
        }

        void testHandleTick_NotIdle() {
            std::unique_ptr<ActualIdleDetector> testling(createDetector());
            testling->setIdleTimeSeconds(15);
            querier->idleTime = 15;
            timerFactory->updateTime(15000);

            querier->idleTime = 5;
            timerFactory->updateTime(30000);

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(idleEvents.size()));
            CPPUNIT_ASSERT(idleEvents[0]);
            CPPUNIT_ASSERT(!idleEvents[1]);
        }

        void testHandleTick_NotIdle_AlreadyNotIdle() {
            std::unique_ptr<ActualIdleDetector> testling(createDetector());
            testling->setIdleTimeSeconds(15);
            querier->idleTime = 5;

            timerFactory->updateTime(15000);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(idleEvents.size()));
        }

    private:
        ActualIdleDetector* createDetector() {
            ActualIdleDetector* detector = new ActualIdleDetector(querier.get(), timerFactory.get(), 10);
            detector->onIdleChanged.connect(boost::bind(&ActualIdleDetectorTest::handleIdle, this, _1));
            return detector;
        }

        void handleIdle(bool b) {
            idleEvents.push_back(b);
        }

    private:
        struct MockIdleQuerier : public IdleQuerier {
            MockIdleQuerier() : idleTime(0) {}
            virtual int getIdleTimeSeconds() { return idleTime; }
            int idleTime;
        };

        struct MockTimer : public Timer {
            MockTimer(int interval) : interval(interval), running(false), lastTime(0) {}

            virtual void start() {
                running = true;
            }

            virtual void stop() {
                running = false;
            }

            virtual void updateTime(int currentTime) {
                if (lastTime == currentTime) {
                    return;
                }
                if (running) {
                    int time = lastTime;
                    while (time <= currentTime) {
                        onTick();
                        time += interval;
                    }
                }
                lastTime = currentTime;
            }

            int interval;
            bool running;
            int lastTime;
        };

        struct MockTimerFactory : public TimerFactory {
            MockTimerFactory() {}

            void updateTime(int milliseconds) {
                for (std::shared_ptr<MockTimer> timer : timers) {
                    timer->updateTime(milliseconds);
                }
            }

            std::shared_ptr<Timer> createTimer(int milliseconds) {
                std::shared_ptr<MockTimer> timer(new MockTimer(milliseconds));
                timers.push_back(timer);
                return timer;
            }

            std::vector<std::shared_ptr<MockTimer> > timers;
        };

        std::unique_ptr<MockIdleQuerier> querier;
        std::unique_ptr<MockTimerFactory> timerFactory;
        std::vector<bool> idleEvents;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ActualIdleDetectorTest);
