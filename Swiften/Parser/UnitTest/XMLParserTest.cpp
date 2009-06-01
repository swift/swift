#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>

#ifdef HAVE_CONFIG_H
#include "Swiften/config.h"
#endif
#include "Swiften/Base/String.h"
#include "Swiften/Parser/XMLParserClient.h"
#ifdef HAVE_EXPAT
#include "Swiften/Parser/ExpatParser.h"
#endif
#ifdef HAVE_LIBXML
#include "Swiften/Parser/LibXMLParser.h"
#endif

using namespace Swift;

template <typename ParserType>
class XMLParserTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(XMLParserTest);
		CPPUNIT_TEST(testParse_NestedElements);
		CPPUNIT_TEST(testParse_CharacterData);
		CPPUNIT_TEST(testParse_NamespacePrefix);
		CPPUNIT_TEST(testParse_UnhandledXML);
		CPPUNIT_TEST(testParse_InvalidXML);
		CPPUNIT_TEST(testParse_InErrorState);
		CPPUNIT_TEST(testParse_Incremental);
		CPPUNIT_TEST_SUITE_END();

	public:
		XMLParserTest() {}

		void testParse_NestedElements() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse(
				"<iq type=\"get\">"
				"<query xmlns='jabber:iq:version'/>"
				"</iq>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(String("iq"), client_.events[0].data);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].attributes.size());
			CPPUNIT_ASSERT_EQUAL(String("get"), client_.events[0].attributes["type"]);

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(String("query"), client_.events[1].data);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[1].attributes.size());
			CPPUNIT_ASSERT_EQUAL(String("jabber:iq:version"), client_.events[1].attributes["xmlns"]);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(String("query"), client_.events[2].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(String("iq"), client_.events[3].data);
		}

		void testParse_CharacterData() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse("<html>bla<i>bli</i>blo</html>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(String("html"), client_.events[0].data);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(String("bla"), client_.events[1].data);

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(String("i"), client_.events[2].data);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(String("bli"), client_.events[3].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[4].type);
			CPPUNIT_ASSERT_EQUAL(String("i"), client_.events[4].data);

			CPPUNIT_ASSERT_EQUAL(Client::CharacterData, client_.events[5].type);
			CPPUNIT_ASSERT_EQUAL(String("blo"), client_.events[5].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[6].type);
			CPPUNIT_ASSERT_EQUAL(String("html"), client_.events[6].data);
		}

		void testParse_NamespacePrefix() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse("<p:x xmlns:p='bla'><p:y/></p:x>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(String("p:x"), client_.events[0].data);

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(String("p:y"), client_.events[1].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[2].type);
			CPPUNIT_ASSERT_EQUAL(String("p:y"), client_.events[2].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[3].type);
			CPPUNIT_ASSERT_EQUAL(String("p:x"), client_.events[3].data);
		}

		void testParse_UnhandledXML() {
			ParserType testling(&client_);

			CPPUNIT_ASSERT(testling.parse("<iq><!-- Testing --></iq>"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), client_.events.size());

			CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
			CPPUNIT_ASSERT_EQUAL(String("iq"), client_.events[0].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(String("iq"), client_.events[1].data);
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
			CPPUNIT_ASSERT_EQUAL(String("iq"), client_.events[0].data);

			CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[1].type);
			CPPUNIT_ASSERT_EQUAL(String("iq"), client_.events[1].data);
		}
	
	private:
		class Client : public XMLParserClient {
			public:
				enum Type { StartElement, EndElement, CharacterData };
				struct Event {
					Event(
							Type type, 
							const String& data, 
							const AttributeMap& attributes)
								: type(type), data(data), attributes(attributes) {}
					Event(Type type, const String& data)
								: type(type), data(data) {}

					Type type;
					String data;
					AttributeMap attributes;
				};

				Client() {}

				virtual void handleStartElement(const String& element, const AttributeMap& attributes) {
					events.push_back(Event(StartElement, element, attributes));
				}

				virtual void handleEndElement(const String& element) {
					events.push_back(Event(EndElement, element));
				}

				virtual void handleCharacterData(const String& data) {
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
