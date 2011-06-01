/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>

#include <string>
#include <Swiften/Parser/XMLParserClient.h>
#ifdef HAVE_EXPAT
#include <Swiften/Parser/ExpatParser.h>
#endif
#ifdef HAVE_LIBXML
#include <Swiften/Parser/LibXMLParser.h>
#endif

using namespace Swift;

template <typename ParserType>
class XMLParserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(XMLParserTest);
		CPPUNIT_TEST(testParse_NestedElements);
		CPPUNIT_TEST(testParse_ElementInNamespacedElement);
		CPPUNIT_TEST(testParse_CharacterData);
		CPPUNIT_TEST(testParse_XMLEntity);
		CPPUNIT_TEST(testParse_NamespacePrefix);
		CPPUNIT_TEST(testParse_UnhandledXML);
		CPPUNIT_TEST(testParse_InvalidXML);
		CPPUNIT_TEST(testParse_InErrorState);
		CPPUNIT_TEST(testParse_Incremental);
		CPPUNIT_TEST(testParse_WhitespaceInAttribute);
		CPPUNIT_TEST(testParse_AttributeWithoutNamespace);
		CPPUNIT_TEST(testParse_AttributeWithNamespace);
		CPPUNIT_TEST(testParse_BillionLaughs);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testParse_NestedElements() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse(
				"<iq type=\"get\">"
				"<query xmlns='jabber:iq:version'/>"
				"</iq>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("iq"), client_.events[0].data);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].attributes.getEntries().size());
			CPPUNIT_ASSERT_EQUAL(std::string("get"), client_.events[0].attributes.getAttribute("type"));
			CPPUNIT_ASSERT_EQUAL(std::string(), client_.events[0].ns);

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("query"), client_.events[1].data);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), client_.events[1].attributes.getEntries().size());
			CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[1].ns);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(std::string("query"), client_.events[2].data);
			CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[2].ns);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(std::string("iq"), client_.events[3].data);
			CPPUNIT_ASSERT_EQUAL(std::string(), client_.events[3].ns);
		}

		void testParse_ElementInNamespacedElement() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse(
				"<query xmlns='jabber:iq:version'>"
					"<name>Swift</name>"
				"</query>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("query"), client_.events[0].data);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), client_.events[0].attributes.getEntries().size());
			CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[0].ns);

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("name"), client_.events[1].data);
			CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[1].ns);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(std::string("Swift"), client_.events[2].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(std::string("name"), client_.events[3].data);
			CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[3].ns);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[4].type);
			CPPUNIT_ASSERT_EQUAL(std::string("query"), client_.events[4].data);
			CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[4].ns);
		}

		void testParse_CharacterData() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse("<html>bla<i>bli</i>blo</html>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("html"), client_.events[0].data);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("bla"), client_.events[1].data);

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(std::string("i"), client_.events[2].data);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(std::string("bli"), client_.events[3].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[4].type);
			CPPUNIT_ASSERT_EQUAL(std::string("i"), client_.events[4].data);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[5].type);
			CPPUNIT_ASSERT_EQUAL(std::string("blo"), client_.events[5].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[6].type);
			CPPUNIT_ASSERT_EQUAL(std::string("html"), client_.events[6].data);
		}

		void testParse_XMLEntity() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse("<html>&lt;&gt;</html>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("html"), client_.events[0].data);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("<"), client_.events[1].data);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(std::string(">"), client_.events[2].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(std::string("html"), client_.events[3].data);
		}

		void testParse_NamespacePrefix() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse("<p:x xmlns:p='bla'><p:y/></p:x>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("x"), client_.events[0].data);
			CPPUNIT_ASSERT_EQUAL(std::string("bla"), client_.events[0].ns);

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("y"), client_.events[1].data);
			CPPUNIT_ASSERT_EQUAL(std::string("bla"), client_.events[1].ns);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(std::string("y"), client_.events[2].data);
			CPPUNIT_ASSERT_EQUAL(std::string("bla"), client_.events[2].ns);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(std::string("x"), client_.events[3].data);
			CPPUNIT_ASSERT_EQUAL(std::string("bla"), client_.events[3].ns);
		}

		void testParse_UnhandledXML() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse("<iq><!-- Testing --></iq>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("iq"), client_.events[0].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("iq"), client_.events[1].data);
		}

		void testParse_InvalidXML() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(!testling.parse("<iq><bla></iq>"));
		}

		void testParse_InErrorState() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(!testling.parse("<iq><bla></iq>"));
			CPPUNIT_ASSERT(!testling.parse("<iq/>"));
		}

		void testParse_Incremental() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse("<iq"));
			CPPUNIT_ASSERT(testling.parse("></iq>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("iq"), client_.events[0].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("iq"), client_.events[1].data);
		}

		void testParse_WhitespaceInAttribute() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse(
				"<query xmlns='http://www.xmpp.org/extensions/xep-0084.html#ns-data '>"));
			CPPUNIT_ASSERT(testling.parse(
				"<presence/>"));
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), client_.events.size());
			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(std::string("query"), client_.events[0].data);
			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(std::string("presence"), client_.events[1].data);
			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(std::string("presence"), client_.events[2].data);
		}

		void testParse_AttributeWithoutNamespace() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse(
				"<query xmlns='http://swift.im' attr='3'/>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].attributes.getEntries().size());
			CPPUNIT_ASSERT_EQUAL(std::string("attr"), client_.events[0].attributes.getEntries()[0].getAttribute().getName());
			CPPUNIT_ASSERT_EQUAL(std::string(""), client_.events[0].attributes.getEntries()[0].getAttribute().getNamespace());
		}

		void testParse_AttributeWithNamespace() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse(
				"<query xmlns='http://swift.im' xmlns:f='http://swift.im/f' f:attr='3'/>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].attributes.getEntries().size());
			CPPUNIT_ASSERT_EQUAL(std::string("attr"), client_.events[0].attributes.getEntries()[0].getAttribute().getName());
			CPPUNIT_ASSERT_EQUAL(std::string("http://swift.im/f"), client_.events[0].attributes.getEntries()[0].getAttribute().getNamespace());
		}

		void testParse_BillionLaughs() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(!testling.parse(
				"<?xml version=\"1.0\"?>"
				"<!DOCTYPE lolz ["
				"  <!ENTITY lol \"lol\">"
				"  <!ENTITY lol2 \"&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;\">"
				"  <!ENTITY lol3 \"&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;&lol2;\">"
				"  <!ENTITY lol4 \"&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;&lol3;\">"
				"  <!ENTITY lol5 \"&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;&lol4;\">"
				"  <!ENTITY lol6 \"&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;&lol5;\">"
				"  <!ENTITY lol7 \"&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;&lol6;\">"
				"  <!ENTITY lol8 \"&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;&lol7;\">"
				"  <!ENTITY lol9 \"&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;\">"
				"]>"
				"<lolz>&lol9;</lolz>"
			));
		}
	
	private:
		class Client : public XMLParserClient {
			public:
				enum Type { StartElement, EndElement, CharacterData };
				struct Event {
					Event(
							Type type, 
							const std::string& data, 
							const std::string& ns,
							const AttributeMap& attributes)
								: type(type), data(data), ns(ns), attributes(attributes) {}
					Event(Type type, const std::string& data, const std::string& ns = std::string())
								: type(type), data(data), ns(ns) {}

					Type type;
					std::string data;
					std::string ns;
					AttributeMap attributes;
				};

				Client() {}

				virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
					events.push_back(Event(StartElement, element, ns, attributes));
				}

				virtual void handleEndElement(const std::string& element, const std::string& ns) {
					events.push_back(Event(EndElement, element, ns));
				}

				virtual void handleCharacterData(const std::string& data) {
					events.push_back(Event(CharacterData, data));
				}

				std::vector<Event> events;
		} client_;
};

#ifdef HAVE_EXPAT
CPPUNIT_TEST_SUITE_REGISTRATION(XMLParserTest<ExpatParser>);
#endif
#ifdef HAVE_LIBXML
CPPUNIT_TEST_SUITE_REGISTRATION(XMLParserTest<LibXMLParser>);
#endif
