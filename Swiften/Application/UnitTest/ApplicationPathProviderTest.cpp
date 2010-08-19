/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Application/PlatformApplicationPathProvider.h"

using namespace Swift;

class ApplicationPathProviderTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ApplicationPathProviderTest);
		CPPUNIT_TEST(testGetSettingsDir);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			testling_ = new PlatformApplicationPathProvider("SwiftTest");
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
		ApplicationPathProvider* testling_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ApplicationPathProviderTest);
