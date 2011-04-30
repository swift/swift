/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParsers/SecurityLabelParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class SecurityLabelParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SecurityLabelParserTest);
		CPPUNIT_TEST(testParse);
		CPPUNIT_TEST_SUITE_END();

	public:
		SecurityLabelParserTest() {}

		void testParse() {
			PayloadsParserTester parser;

			CPPUNIT_ASSERT(parser.parse(
				"<securitylabel xmlns=\"urn:xmpp:sec-label:0\">"
					"<displaymarking fgcolor=\"black\" bgcolor=\"red\">SECRET</displaymarking>"
					"<label>"
						"<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel>"
					"</label>"
					"<equivalentlabel>"
						"<icismlabel xmlns=\"http://example.gov/IC-ISM/0\" classification=\"S\" ownerProducer=\"USA\" disseminationControls=\"FOUO\"/>"
					"</equivalentlabel>"
					"<equivalentlabel>"
						"<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MRUCAgD9DA9BcXVhIChvYnNvbGV0ZSk=</esssecuritylabel>"
					"</equivalentlabel>"
				"</securitylabel>"));

			SecurityLabel* payload = dynamic_cast<SecurityLabel*>(parser.getPayload().get());
			CPPUNIT_ASSERT_EQUAL(std::string("SECRET"), payload->getDisplayMarking());
			CPPUNIT_ASSERT_EQUAL(std::string("black"), payload->getForegroundColor());
			CPPUNIT_ASSERT_EQUAL(std::string("red"), payload->getBackgroundColor());
			CPPUNIT_ASSERT_EQUAL(std::string("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel>"), payload->getLabel());	
			CPPUNIT_ASSERT_EQUAL(std::string("<icismlabel classification=\"S\" disseminationControls=\"FOUO\" ownerProducer=\"USA\" xmlns=\"http://example.gov/IC-ISM/0\"/>"), payload->getEquivalentLabels()[0]);	
			CPPUNIT_ASSERT_EQUAL(std::string("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MRUCAgD9DA9BcXVhIChvYnNvbGV0ZSk=</esssecuritylabel>"), payload->getEquivalentLabels()[1]);	
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SecurityLabelParserTest);
