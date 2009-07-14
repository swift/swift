#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Base/String.h"
#include "Swiften/Base/sleep.h"
#include "Swiften/Network/BoostConnection.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/EventLoop/DummyEventLoop.h"

using namespace Swift;

class BoostConnectionTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(BoostConnectionTest);
		CPPUNIT_TEST(testDestructor);
		CPPUNIT_TEST(testDestructor_PendingEvents);
		CPPUNIT_TEST_SUITE_END();

	public:
		BoostConnectionTest() {}

		void setUp() {
			eventLoop_ = new DummyEventLoop();
			boostIOServiceThread_ = new BoostIOServiceThread();
		}

		void tearDown() {
			delete boostIOServiceThread_;
			delete eventLoop_;
		}

		void testDestructor() {
			{
				std::string domain("el-tramo.be");
				std::auto_ptr<BoostConnection> testling(new BoostConnection(&boostIOServiceThread_->getIOService()));
				testling->connect(domain);
			}
		}

		void testDestructor_PendingEvents() {
			{
				std::auto_ptr<BoostConnection> testling(new BoostConnection(&boostIOServiceThread_->getIOService()));
				testling->connect("el-tramo.be");
				while (!eventLoop_->hasEvents()) {
					Swift::sleep(10);
				}
			}
			eventLoop_->processEvents();
		}

	private:
		BoostIOServiceThread* boostIOServiceThread_;
		DummyEventLoop* eventLoop_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoostConnectionTest);
