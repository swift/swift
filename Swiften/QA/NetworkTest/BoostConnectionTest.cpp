#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Base/String.h"
#include "Swiften/Base/sleep.h"
#include "Swiften/Network/BoostConnection.h"
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
    }

    void tearDown() {
      delete eventLoop_;
    }

		void testDestructor() {
      {
        std::string domain("el-tramo.be");
        std::auto_ptr<BoostConnection> testling(new BoostConnection(domain));
        testling->connect();
      }
    }

		void testDestructor_PendingEvents() {
      {
        std::auto_ptr<BoostConnection> testling(new BoostConnection("el-tramo.be"));
        testling->connect();
        while (!eventLoop_->hasEvents()) {
          Swift::sleep(10);
        }
      }
      eventLoop_->processEvents();
		}

  private:
    DummyEventLoop* eventLoop_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoostConnectionTest);
