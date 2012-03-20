/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/StanzaParser.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Elements/Payload.h>

using namespace Swift;

class StanzaParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(StanzaParserTest);
		CPPUNIT_TEST(testHandleEndElement_OnePayload);
		CPPUNIT_TEST(testHandleEndElement_MultiplePayloads);
		CPPUNIT_TEST(testHandleEndElement_StrayCharacterData);
		CPPUNIT_TEST(testHandleEndElement_UnknownPayload);
		CPPUNIT_TEST(testHandleParse_BasicAttributes);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			factoryCollection_ = new PayloadParserFactoryCollection();
			factoryCollection_->addFactory(&factory1_);
			factoryCollection_->addFactory(&factory2_);
		}

		void tearDown() {
			delete factoryCollection_;
		}

		void testHandleEndElement_OnePayload() {
			MyStanzaParser testling(factoryCollection_);

			AttributeMap attributes;
			attributes.addAttribute("foo", "", "fum");
			attributes.addAttribute("bar", "", "baz");
			testling.handleStartElement("mystanza", "", attributes);
			testling.handleStartElement("mypayload1", "", attributes);
			testling.handleStartElement("child", "", attributes);
			testling.handleEndElement("child", "");
			testling.handleEndElement("mypayload1", "");
			testling.handleEndElement("mystanza", "");

			CPPUNIT_ASSERT(testling.getStanza()->getPayload<MyPayload1>());
			CPPUNIT_ASSERT(testling.getStanza()->getPayload<MyPayload1>()->hasChild);
		}

		void testHandleEndElement_MultiplePayloads() {
			MyStanzaParser testling(factoryCollection_);

			AttributeMap attributes;
			testling.handleStartElement("mystanza", "", attributes);
			testling.handleStartElement("mypayload1", "", attributes);
			testling.handleEndElement("mypayload1", "");
			testling.handleStartElement("mypayload2", "", attributes);
			testling.handleEndElement("mypayload2", "");
			testling.handleEndElement("mystanza", "");

			CPPUNIT_ASSERT(testling.getStanza()->getPayload<MyPayload1>());
			CPPUNIT_ASSERT(testling.getStanza()->getPayload<MyPayload2>());
		}

		void testHandleEndElement_StrayCharacterData() {
			MyStanzaParser testling(factoryCollection_);

			AttributeMap attributes;
			testling.handleStartElement("mystanza", "", attributes);
			testling.handleStartElement("mypayload1", "", attributes);
			testling.handleEndElement("mypayload1", "");
			testling.handleCharacterData("bla");
			testling.handleStartElement("mypayload2", "", attributes);
			testling.handleEndElement("mypayload2", "");
			testling.handleEndElement("mystanza", "");

			CPPUNIT_ASSERT(testling.getStanza()->getPayload<MyPayload1>());
			CPPUNIT_ASSERT(testling.getStanza()->getPayload<MyPayload2>());
		}

		void testHandleEndElement_UnknownPayload() {
			MyStanzaParser testling(factoryCollection_);

			AttributeMap attributes;
			testling.handleStartElement("mystanza", "", attributes);
			testling.handleStartElement("mypayload1", "", attributes);
			testling.handleEndElement("mypayload1", "");
			testling.handleStartElement("unknown-payload", "", attributes);
			testling.handleStartElement("unknown-payload-child", "", attributes);
			testling.handleEndElement("unknown-payload-child", "");
			testling.handleEndElement("unknown-payload", "");
			testling.handleStartElement("mypayload2", "", attributes);
			testling.handleEndElement("mypayload2", "");
			testling.handleEndElement("mystanza", "");

			CPPUNIT_ASSERT(testling.getStanza()->getPayload<MyPayload1>());
			CPPUNIT_ASSERT(testling.getStanza()->getPayload<MyPayload2>());
		}

		void testHandleParse_BasicAttributes() {
			MyStanzaParser testling(factoryCollection_);

			AttributeMap attributes;
			attributes.addAttribute("to", "", "foo@example.com/blo");
			attributes.addAttribute("from", "", "bar@example.com/baz");
			attributes.addAttribute("id", "", "id-123");
			testling.handleStartElement("mystanza", "", attributes);
			testling.handleEndElement("mypayload1", "");

			CPPUNIT_ASSERT_EQUAL(JID("foo@example.com/blo"), testling.getStanza()->getTo());
			CPPUNIT_ASSERT_EQUAL(JID("bar@example.com/baz"), testling.getStanza()->getFrom());
			CPPUNIT_ASSERT_EQUAL(std::string("id-123"), testling.getStanza()->getID());
		}

	private:
		class MyPayload1 : public Payload
		{
			public:
				MyPayload1() : hasChild(false) {}

				bool hasChild;
		};

		class MyPayload1Parser : public GenericPayloadParser<MyPayload1>
		{
			public:	
				MyPayload1Parser() {}

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap&) { 
				if (element != "mypayload1") {
					getPayloadInternal()->hasChild = true;
				}
			}

			virtual void handleEndElement(const std::string&, const std::string&) {}
			virtual void handleCharacterData(const std::string&) {}
		};

		class MyPayload1ParserFactory : public PayloadParserFactory
		{
			public:
				MyPayload1ParserFactory() {}

				PayloadParser* createPayloadParser() { return new MyPayload1Parser(); }

				bool canParse(const std::string& element, const std::string&, const AttributeMap&) const {
					return element == "mypayload1";
				}
		};

		class MyPayload2 : public Payload
		{
			public:
				MyPayload2() {}
		};

		class MyPayload2Parser : public GenericPayloadParser<MyPayload2>
		{
			public:	
				MyPayload2Parser() {}

			virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&) {}
			virtual void handleEndElement(const std::string&, const std::string&) {}
			virtual void handleCharacterData(const std::string&) {}
		};


		class MyPayload2ParserFactory : public PayloadParserFactory
		{
			public:
				MyPayload2ParserFactory() {}

				PayloadParser* createPayloadParser() { return new MyPayload2Parser(); }
				bool canParse(const std::string& element, const std::string&, const AttributeMap&) const {
					return element == "mypayload2";
				}
		};

		class MyStanza : public Stanza
		{
			public:
				MyStanza() {}
		};

		class MyStanzaParser : public StanzaParser
		{
			public:
				MyStanzaParser(PayloadParserFactoryCollection* collection) : StanzaParser(collection)
				{
					stanza_ = boost::make_shared<MyStanza>();
				}
			
				virtual boost::shared_ptr<Element> getElement() const {
					return stanza_;
				}

			private:
				boost::shared_ptr<MyStanza> stanza_;
		};
	
		MyPayload1ParserFactory factory1_;
		MyPayload2ParserFactory factory2_;
		PayloadParserFactoryCollection* factoryCollection_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(StanzaParserTest);
