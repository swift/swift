/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Elements/StreamInitiation.h>

using namespace Swift;

class StreamInitiationParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(StreamInitiationParserTest);
		CPPUNIT_TEST(testParse_Request);
		CPPUNIT_TEST(testParse_Response);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse_Request() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
					"<si xmlns='http://jabber.org/protocol/si' id='a0' mime-type='text/plain' profile='http://jabber.org/protocol/si/profile/file-transfer'>"
						"<file xmlns='http://jabber.org/protocol/si/profile/file-transfer' name='test.txt' size='1022'>"
					    "<desc>This is info about the file.</desc>"
						"</file>"
						"<feature xmlns='http://jabber.org/protocol/feature-neg'>"
							"<x xmlns='jabber:x:data' type='form'>"
								"<field var='stream-method' type='list-single'>"
									"<option><value>http://jabber.org/protocol/bytestreams</value></option>"
									"<option><value>jabber:iq:oob</value></option>"
									"<option><value>http://jabber.org/protocol/ibb</value></option>"
								"</field>"
							"</x>"
						"</feature>"
					"</si>"
			));

			StreamInitiation::ref si = parser.getPayload<StreamInitiation>();
			CPPUNIT_ASSERT(si->getIsFileTransfer());
			CPPUNIT_ASSERT(si->getFileInfo());
			CPPUNIT_ASSERT_EQUAL(std::string("test.txt"), si->getFileInfo()->getName());
			CPPUNIT_ASSERT(1022 == si->getFileInfo()->getSize());
			CPPUNIT_ASSERT_EQUAL(std::string("This is info about the file."), si->getFileInfo()->getDescription());
			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(si->getProvidedMethods().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("http://jabber.org/protocol/bytestreams"), si->getProvidedMethods()[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:oob"), si->getProvidedMethods()[1]);
			CPPUNIT_ASSERT_EQUAL(std::string("http://jabber.org/protocol/ibb"), si->getProvidedMethods()[2]);
		}

		void testParse_Response() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
					"<si xmlns='http://jabber.org/protocol/si'>"
						"<feature xmlns='http://jabber.org/protocol/feature-neg'>"
							"<x xmlns='jabber:x:data' type='submit'>"
								"<field var='stream-method'>"
									"<value>http://jabber.org/protocol/bytestreams</value>"
								"</field>"
							"</x>"
						"</feature>"
					"</si>"
			));

			StreamInitiation::ref si = parser.getPayload<StreamInitiation>();
			CPPUNIT_ASSERT(si->getIsFileTransfer());
			CPPUNIT_ASSERT_EQUAL(std::string("http://jabber.org/protocol/bytestreams"), si->getRequestedMethod());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamInitiationParserTest);
