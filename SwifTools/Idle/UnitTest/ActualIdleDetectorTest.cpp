/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include <SwifTools/Idle/ActualIdleDetector.h>
#include <SwifTools/Idle/IdleQuerier.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Network/Timer.h>

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
			querier = new MockIdleQuerier();
			timerFactory = new MockTimerFactory();
			idleEvents.clear();
		}

		void tearDown() {
			delete timerFactory;
			delete querier;
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
			std::auto_ptr<ActualIdleDetector> testling(createDetector());
			testling->setIdleTimeSeconds(15);
			querier->idleTime = 15;

			timerFactory->updateTime(15000);
			
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(idleEvents.size()));
			CPPUNIT_ASSERT(idleEvents[0]);
		}

		void testHandleTick_Idle_AlreadyIdle() {
			std::auto_ptr<ActualIdleDetector> testling(createDetector());
			testling->setIdleTimeSeconds(15);
			querier->idleTime = 15;
			timerFactory->updateTime(15000);

			querier->idleTime = 30;
			timerFactory->updateTime(30000);
			
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(idleEvents.size()));
			CPPUNIT_ASSERT(idleEvents[0]);
		}

		void testHandleTick_NotIdle() {
			std::auto_ptr<ActualIdleDetector> testling(createDetector());
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
			std::auto_ptr<ActualIdleDetector> testling(createDetector());
			testling->setIdleTimeSeconds(15);
			querier->idleTime = 5;

			timerFactory->updateTime(15000);
			
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(idleEvents.size()));
		}
	
	private:
		ActualIdleDetector* createDetector() {
			ActualIdleDetector* detector = new ActualIdleDetector(querier, timerFactory, 10);
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
				foreach(boost::shared_ptr<MockTimer> timer, timers) {
					timer->updateTime(milliseconds);
				}
			}

			boost::shared_ptr<Timer> createTimer(int milliseconds) {
				boost::shared_ptr<MockTimer> timer(new MockTimer(milliseconds));
				timers.push_back(timer);
				return timer;
			}

			std::vector<boost::shared_ptr<MockTimer> > timers;
		};

		MockIdleQuerier* querier;
		MockTimerFactory* timerFactory;
		std::vector<bool> idleEvents;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ActualIdleDetectorTest);
