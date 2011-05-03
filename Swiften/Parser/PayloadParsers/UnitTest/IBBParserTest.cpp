/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Elements/IBB.h>

using namespace Swift;

class IBBParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(IBBParserTest);
		CPPUNIT_TEST(testParse_Data);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse_Data() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
					"<data xmlns='http://jabber.org/protocol/ibb' seq='4'>\n"
					"\t  YWJjZGVmZ2loamtsbW5vcHFyc3R1dnd4eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjEyMzQ1\n"
					"\t  Njc4OTAK\n"
					"</data>"
			));

			IBB::ref ibb = parser.getPayload<IBB>();
			CPPUNIT_ASSERT(ibb->getAction() == IBB::Data);
			CPPUNIT_ASSERT(createByteArray("abcdefgihjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890\x0a") == ibb->getData());
			CPPUNIT_ASSERT_EQUAL(4, ibb->getSequenceNumber());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(IBBParserTest);
