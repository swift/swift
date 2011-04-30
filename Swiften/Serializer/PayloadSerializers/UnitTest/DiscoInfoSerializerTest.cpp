/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/DiscoInfoSerializer.h>

using namespace Swift;

class DiscoInfoSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(DiscoInfoSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerialize_Form);
		CPPUNIT_TEST_SUITE_END();

	public:
		DiscoInfoSerializerTest() {}

		void testSerialize() {
			DiscoInfoSerializer testling;
			boost::shared_ptr<DiscoInfo> discoInfo(new DiscoInfo());
			discoInfo->addIdentity(DiscoInfo::Identity("Swift", "client", "pc"));
			discoInfo->addIdentity(DiscoInfo::Identity("Vlug", "client", "pc", "nl"));
			discoInfo->addFeature("http://jabber.org/protocol/caps");
			discoInfo->addFeature("http://jabber.org/protocol/disco#info");
			discoInfo->setNode("http://swift.im#bla");

			std::string expectedResult = 
				"<query node=\"http://swift.im#bla\" xmlns=\"http://jabber.org/protocol/disco#info\">"
					"<identity category=\"client\" name=\"Swift\" type=\"pc\"/>"
					"<identity category=\"client\" name=\"Vlug\" type=\"pc\" xml:lang=\"nl\"/>"
					"<feature var=\"http://jabber.org/protocol/caps\"/>"
					"<feature var=\"http://jabber.org/protocol/disco#info\"/>"
				"</query>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(discoInfo));
		}

		void testSerialize_Form() {
			DiscoInfoSerializer testling;
			boost::shared_ptr<DiscoInfo> discoInfo(new DiscoInfo());
			discoInfo->addFeature("http://jabber.org/protocol/caps");
			discoInfo->addFeature("http://jabber.org/protocol/disco#info");
			boost::shared_ptr<Form> form(new Form(Form::FormType));
			form->setTitle("Bot Configuration");
			discoInfo->addExtension(form);

			std::string expectedResult = 
				"<query xmlns=\"http://jabber.org/protocol/disco#info\">"
					"<feature var=\"http://jabber.org/protocol/caps\"/>"
					"<feature var=\"http://jabber.org/protocol/disco#info\"/>"
					"<x type=\"form\" xmlns=\"jabber:x:data\">"
							"<title>Bot Configuration</title>"
					"</x>"
				"</query>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(discoInfo));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(DiscoInfoSerializerTest);
