/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <string>
#include <boost/algorithm/string.hpp>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>

using namespace Swift;

class ApplicationPathProviderTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ApplicationPathProviderTest);
		CPPUNIT_TEST(testGetDataDir);
		CPPUNIT_TEST(testGetExecutableDir);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			testling_ = new PlatformApplicationPathProvider("SwiftTest");
		}

		void tearDown() {
			delete testling_;
		}

		void testGetDataDir() {
			boost::filesystem::path dir = testling_->getDataDir();

			CPPUNIT_ASSERT(boost::filesystem::exists(dir));
			CPPUNIT_ASSERT(boost::filesystem::is_directory(dir));

			boost::filesystem::remove(dir);
		}

		void testGetExecutableDir() {
			boost::filesystem::path dir = testling_->getExecutableDir();
			CPPUNIT_ASSERT(boost::filesystem::is_directory(dir));
			CPPUNIT_ASSERT(boost::ends_with(dir.string(), "UnitTest"));
		}
	
	private:
		ApplicationPathProvider* testling_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ApplicationPathProviderTest);
