/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/PayloadSerializers/SecurityLabelsCatalogSerializer.h"

using namespace Swift;

class SecurityLabelsCatalogSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(SecurityLabelsCatalogSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		SecurityLabelsCatalogSerializerTest() {}

		void testSerialize() {
			SecurityLabelsCatalogSerializer testling;
			boost::shared_ptr<SecurityLabelsCatalog> catalog(new SecurityLabelsCatalog());
			catalog->setTo(JID("example.com"));
			catalog->setName("Default");
			catalog->setDescription("an example set of labels");

			SecurityLabel securityLabel1;
			securityLabel1.setDisplayMarking("SECRET");
			securityLabel1.setForegroundColor("black");
			securityLabel1.setBackgroundColor("red");
			securityLabel1.setLabel("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel>");
			catalog->addLabel(securityLabel1);

			SecurityLabel securityLabel2;
			securityLabel2.setDisplayMarking("CONFIDENTIAL");
			securityLabel2.setForegroundColor("black");
			securityLabel2.setBackgroundColor("navy");
			securityLabel2.setLabel("<esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQMGASk=</esssecuritylabel>");
			catalog->addLabel(securityLabel2);

			CPPUNIT_ASSERT_EQUAL(String(
				"<catalog desc=\"an example set of labels\" name=\"Default\" to=\"example.com\" xmlns=\"urn:xmpp:sec-label:catalog:0\">"
					"<securitylabel xmlns=\"urn:xmpp:sec-label:0\">"
						"<displaymarking bgcolor=\"red\" fgcolor=\"black\">SECRET</displaymarking>"
						"<label><esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQYCAQQGASk=</esssecuritylabel></label>"
					"</securitylabel>"
					"<securitylabel xmlns=\"urn:xmpp:sec-label:0\">"
						"<displaymarking bgcolor=\"navy\" fgcolor=\"black\">CONFIDENTIAL</displaymarking>"
						"<label><esssecuritylabel xmlns=\"urn:xmpp:sec-label:ess:0\">MQMGASk=</esssecuritylabel></label>"
					"</securitylabel>"
				"</catalog>"), testling.serialize(catalog));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SecurityLabelsCatalogSerializerTest);
