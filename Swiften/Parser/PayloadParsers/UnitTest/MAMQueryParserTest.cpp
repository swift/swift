/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/PayloadParsers/MAMQueryParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class MAMQueryParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(MAMQueryParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST(testParseEmpty);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse(
				"<query queryid=\"id0\" xmlns=\"urn:xmpp:mam:0\">"
					"<x type=\"form\" xmlns=\"jabber:x:data\">"
						"<field type=\"text-single\" var=\"FORM_TYPE\">"
							"<value>urn:xmpp:mam:0</value>"
						"</field>"
						"<field type=\"text-single\" var=\"start\">"
							"<value>2010-08-07T00:00:00Z</value>"
						"</field>"
					"</x>"
					"<set xmlns=\"http://jabber.org/protocol/rsm\">"
						"<max>10</max>"
					"</set>"
				"</query>"));


			boost::shared_ptr<MAMQuery> payload = parser.getPayload<MAMQuery>();
			CPPUNIT_ASSERT(!!payload);
			CPPUNIT_ASSERT(payload->getQueryID());
			CPPUNIT_ASSERT_EQUAL(std::string("id0"), *payload->getQueryID());

			CPPUNIT_ASSERT(payload->getForm());
			boost::shared_ptr<FormField> fieldType = payload->getForm()->getField("FORM_TYPE");
			CPPUNIT_ASSERT(fieldType);
			CPPUNIT_ASSERT_EQUAL(std::string("urn:xmpp:mam:0"), fieldType->getTextSingleValue());
			boost::shared_ptr<FormField> fieldStart = payload->getForm()->getField("start");
			CPPUNIT_ASSERT(fieldStart);
			CPPUNIT_ASSERT_EQUAL(std::string("2010-08-07T00:00:00Z"), fieldStart->getTextSingleValue());

			CPPUNIT_ASSERT(payload->getResultSet());
			boost::shared_ptr<ResultSet> resultSet = payload->getResultSet();
			CPPUNIT_ASSERT(resultSet->getMaxItems());
			CPPUNIT_ASSERT_EQUAL(*resultSet->getMaxItems(), 10);
		}

		void testParseEmpty() {
			PayloadsParserTester parser;
			CPPUNIT_ASSERT(parser.parse(
				"<query queryid=\"id0\" xmlns=\"urn:xmpp:mam:0\">"
				"</query>"));

			boost::shared_ptr<MAMQuery> payload = parser.getPayload<MAMQuery>();
			CPPUNIT_ASSERT(!!payload);
			CPPUNIT_ASSERT(payload->getQueryID());
			CPPUNIT_ASSERT_EQUAL(std::string("id0"), *payload->getQueryID());
			CPPUNIT_ASSERT(!payload->getForm());
			CPPUNIT_ASSERT(!payload->getResultSet());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(MAMQueryParserTest);
