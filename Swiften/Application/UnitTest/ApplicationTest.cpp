#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Application/Application.h"
#include "Swiften/Application/Platform/PlatformApplication.h"

using namespace Swift;

class ApplicationTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(ApplicationTest);
		CPPUNIT_TEST(testGetSettingsDir);
		CPPUNIT_TEST_SUITE_END();

	public:
		ApplicationTest() {}

		void setUp() {
			testling_ = new PlatformApplication("SwiftTest");
		}

		void tearDown() {
			delete testling_;
		}

		void testGetSettingsDir() {
			boost::filesystem::path dir = testling_->getSettingsDir();

			CPPUNIT_ASSERT(boost::filesystem::exists(dir));
			CPPUNIT_ASSERT(boost::filesystem::is_directory(dir));

			boost::filesystem::remove(dir);
		}
	
	private:
		Application* testling_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ApplicationTest);
