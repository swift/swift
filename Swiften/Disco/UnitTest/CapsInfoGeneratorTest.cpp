/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/Disco/CapsInfoGenerator.h>
#include <Swiften/Elements/DiscoInfo.h>

using namespace Swift;

class CapsInfoGeneratorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CapsInfoGeneratorTest);
        CPPUNIT_TEST(testGenerate_XEP0115SimpleExample);
        CPPUNIT_TEST(testGenerate_XEP0115ComplexExample);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
        }

        void testGenerate_XEP0115SimpleExample() {
            DiscoInfo discoInfo;
            discoInfo.addIdentity(DiscoInfo::Identity("Exodus 0.9.1", "client", "pc"));
            discoInfo.addFeature("http://jabber.org/protocol/disco#items");
            discoInfo.addFeature("http://jabber.org/protocol/caps");
            discoInfo.addFeature("http://jabber.org/protocol/disco#info");
            discoInfo.addFeature("http://jabber.org/protocol/muc");

            CapsInfoGenerator testling("http://code.google.com/p/exodus", crypto.get());
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
            FormField::ref field = std::make_shared<FormField>(FormField::HiddenType, "urn:xmpp:dataforms:softwareinfo");
            field->setName("FORM_TYPE");
            extension->addField(field);
            field = std::make_shared<FormField>(FormField::ListMultiType);
            field->addValue("ipv6");
            field->addValue("ipv4");
            field->setName("ip_version");
            extension->addField(field);
            field = std::make_shared<FormField>(FormField::TextSingleType, "Psi");
            field->setName("software");
            extension->addField(field);
            field = std::make_shared<FormField>(FormField::TextSingleType, "0.11");
            field->setName("software_version");
            extension->addField(field);
            field = std::make_shared<FormField>(FormField::TextSingleType, "Mac");
            field->setName("os");
            extension->addField(field);
            field = std::make_shared<FormField>(FormField::TextSingleType, "10.5.1");
            field->setName("os_version");
            extension->addField(field);
            discoInfo.addExtension(extension);

            CapsInfoGenerator testling("http://psi-im.org", crypto.get());
            CapsInfo result = testling.generateCapsInfo(discoInfo);

            CPPUNIT_ASSERT_EQUAL(std::string("q07IKJEyjvHSyhy//CH0CxmKi8w="), result.getVersion());
        }

    private:
        std::shared_ptr<CryptoProvider> crypto;
};

CPPUNIT_TEST_SUITE_REGISTRATION(CapsInfoGeneratorTest);
