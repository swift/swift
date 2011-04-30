/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/SecurityLabelsCatalogParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class SecurityLabelsCatalogParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SecurityLabelsCatalogParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		SecurityLabelsCatalogParserTest() {}

		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<catalog desc=\"an example set of labels\" name=\"Default\" to=\"example.com\" xmlns=\"urn:xmpp:sec-label:catalog:2\">"
				 "<item selector='Classified|SECRET'>"
					"<securitylabel xmlns=\"urn:xmpp:sec-label:0\">"
						"<displaymarking bgcolor=\"red\" fgcolor=\"black\">SECRET</displaymarking>"
						"<label><esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel></label>"
					"</securitylabel>"
				 "</item>"
				 "<item selector='Classified|CONFIDENTIAL' default='true'>"
					"<securitylabel xmlns=\"urn:xmpp:sec-label:0\">"
						"<displaymarking bgcolor=\"navy\" fgcolor=\"black\">CONFIDENTIAL</displaymarking>"
						"<label><esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQMGASk=</esssecuritylabel></label>"
					"</securitylabel>"
				 "</item>"
				 "<item selector='Unclassified|UNCLASSIFIED'/>"
				"</catalog>"));

			SecurityLabelsCatalog* payload = dynamic_cast<SecurityLabelsCatalog*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(std::string("Default"), payload->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("an example set of labels"), payload->getDescription());
			CPPUNIT_ASSERT_EQUAL(JID("example.com"), payload->getTo());
			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(payload->getItems().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("SECRET"), payload->getItems()[0].getLabel()->getDisplayMarking());
			CPPUNIT_ASSERT_EQUAL(std::string("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel>"), payload->getItems()[0].getLabel()->getLabel());
			CPPUNIT_ASSERT_EQUAL(false, payload->getItems()[0].getIsDefault());
			CPPUNIT_ASSERT_EQUAL(std::string("Classified|SECRET"), payload->getItems()[0].getSelector());
			CPPUNIT_ASSERT_EQUAL(std::string("CONFIDENTIAL"), payload->getItems()[1].getLabel()->getDisplayMarking());
			CPPUNIT_ASSERT_EQUAL(std::string("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQMGASk=</esssecuritylabel>"), payload->getItems()[1].getLabel()->getLabel());
			CPPUNIT_ASSERT_EQUAL(true, payload->getItems()[1].getIsDefault());
			CPPUNIT_ASSERT_EQUAL(std::string("Classified|CONFIDENTIAL"), payload->getItems()[1].getSelector());
			CPPUNIT_ASSERT_EQUAL(false, payload->getItems()[2].getIsDefault());
			CPPUNIT_ASSERT_EQUAL(std::string("Unclassified|UNCLASSIFIED"), payload->getItems()[2].getSelector());
			CPPUNIT_ASSERT(!payload->getItems()[2].getLabel());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SecurityLabelsCatalogParserTest);
