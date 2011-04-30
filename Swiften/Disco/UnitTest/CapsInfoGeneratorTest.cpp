/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Disco/CapsInfoGenerator.h>

using namespace Swift;

class CapsInfoGeneratorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(CapsInfoGeneratorTest);
		CPPUNIT_TEST(testGenerate_XEP0115SimpleExample);
		CPPUNIT_TEST(testGenerate_XEP0115ComplexExample);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGenerate_XEP0115SimpleExample() {
			DiscoInfo discoInfo;
			discoInfo.addIdentity(DiscoInfo::Identity("Exodus 0.9.1", "client", "pc"));
			discoInfo.addFeature("http://jabber.org/protocol/disco#items");
			discoInfo.addFeature("http://jabber.org/protocol/caps");
			discoInfo.addFeature("http://jabber.org/protocol/disco#info");
			discoInfo.addFeature("http://jabber.org/protocol/muc");

			CapsInfoGenerator testling("http://code.google.com/p/exodus");
			CapsInfo result = testling.generateCapsInfo(discoInfo);

			CPPUNIT_ASSERT_EQUAL(std::string("http://code.google.com/p/exodus"), result.getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("sha-1"), result.getHash());
			CPPUNIT_ASSERT_EQUAL(std::string("QgayPKawpkPSDYmwT/WM94uAlu0="), result.getVersion());
		}

		void testGenerate_XEP0115ComplexExample() {
			DiscoInfo discoInfo;
			discoInfo.addIdentity(DiscoInfo::Identity("Psi 0.11", "client", "pc", "en"));
			discoInfo.addIdentity(DiscoInfo::Identity("\xce\xa8 0.11", "client", "pc", "el"));
			discoInfo.addFeature("http://jabber.org/protocol/disco#items");
			discoInfo.addFeature("http://jabber.org/protocol/caps");
			discoInfo.addFeature("http://jabber.org/protocol/disco#info");
			discoInfo.addFeature("http://jabber.org/protocol/muc");

			Form::ref extension(new Form(Form::ResultType));
			FormField::ref field = HiddenFormField::create("urn:xmpp:dataforms:softwareinfo");
			field->setName("FORM_TYPE");
			extension->addField(field);
			std::vector<std::string> ipVersions;
			ipVersions.push_back("ipv6");
			ipVersions.push_back("ipv4");
			field = ListMultiFormField::create(ipVersions);
			field->addRawValue("ipv6");
			field->addRawValue("ipv4");
			field->setName("ip_version");
			extension->addField(field);
			field = TextSingleFormField::create("Psi");
			field->addRawValue("Psi");
			field->setName("software");
			extension->addField(field);
			field = TextSingleFormField::create("0.11");
			field->addRawValue("0.11");
			field->setName("software_version");
			extension->addField(field);
			field = TextSingleFormField::create("Mac");
			field->setName("os");
			field->addRawValue("Mac");
			extension->addField(field);
			field = TextSingleFormField::create("10.5.1");
			field->setName("os_version");
			field->addRawValue("10.5.1");
			extension->addField(field);
			discoInfo.addExtension(extension);

			CapsInfoGenerator testling("http://psi-im.org");
			CapsInfo result = testling.generateCapsInfo(discoInfo);

			CPPUNIT_ASSERT_EQUAL(std::string("q07IKJEyjvHSyhy//CH0CxmKi8w="), result.getVersion());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(CapsInfoGeneratorTest);
