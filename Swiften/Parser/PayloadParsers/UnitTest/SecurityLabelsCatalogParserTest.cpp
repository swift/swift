/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Parser/PayloadParsers/SecurityLabelsCatalogParser.h"
#include "Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h"

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
				"<catalog desc=\"an example set of labels\" name=\"Default\" to=\"example.com\" xmlns=\"urn:xmpp:sec-label:catalog:0\">"
					"<securitylabel xmlns=\"urn:xmpp:sec-label:0\">"
						"<displaymarking bgcolor=\"red\" fgcolor=\"black\">SECRET</displaymarking>"
						"<label><esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel></label>"
					"</securitylabel>"
					"<securitylabel xmlns=\"urn:xmpp:sec-label:0\">"
						"<displaymarking bgcolor=\"navy\" fgcolor=\"black\">CONFIDENTIAL</displaymarking>"
						"<label><esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQMGASk=</esssecuritylabel></label>"
					"</securitylabel>"
				"</catalog>"));

			SecurityLabelsCatalog* payload = dynamic_cast<SecurityLabelsCatalog*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(std::string("Default"), payload->getName());
			CPPUNIT_ASSERT_EQUAL(std::string("an example set of labels"), payload->getDescription());
			CPPUNIT_ASSERT_EQUAL(JID("example.com"), payload->getTo());
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(payload->getLabels().size()));
			CPPUNIT_ASSERT_EQUAL(std::string("SECRET"), payload->getLabels()[0].getDisplayMarking());
			CPPUNIT_ASSERT_EQUAL(std::string("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel>"), payload->getLabels()[0].getLabel());
			CPPUNIT_ASSERT_EQUAL(std::string("CONFIDENTIAL"), payload->getLabels()[1].getDisplayMarking());
			CPPUNIT_ASSERT_EQUAL(std::string("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQMGASk=</esssecuritylabel>"), payload->getLabels()[1].getLabel());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SecurityLabelsCatalogParserTest);
