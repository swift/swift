/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <unordered_map>
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
        CPPUNIT_TEST(testParse_AttributeWithNamespaceNoPrefix);
        CPPUNIT_TEST(testParse_BillionLaughs);
        CPPUNIT_TEST(testParse_InternalEntity);
        //CPPUNIT_TEST(testParse_UndefinedPrefix);
        //CPPUNIT_TEST(testParse_UndefinedAttributePrefix);
        CPPUNIT_TEST(testParse_AllowCommentsInXML);
        CPPUNIT_TEST(testParse_DisallowCommentsInXML);
        CPPUNIT_TEST(testParse_Doctype);
        CPPUNIT_TEST(testParse_ProcessingInstructions);
        CPPUNIT_TEST(testParse_ProcessingPrefixedElement);
        CPPUNIT_TEST(testParse_InvalidlyEncodedInput);
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
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[1].namespaces.size());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[1].namespaces.count(""));
            CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[1].namespaces[""]);

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
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].namespaces.size());
            CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[0].namespaces[""]);

            CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[1].type);
            CPPUNIT_ASSERT_EQUAL(std::string("name"), client_.events[1].data);
            CPPUNIT_ASSERT_EQUAL(std::string("jabber:iq:version"), client_.events[1].ns);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), client_.events[1].namespaces.size());

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
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].namespaces.size());
            CPPUNIT_ASSERT_EQUAL(std::string("bla"), client_.events[0].namespaces["p"]);

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
            ParserType testling(&client_, true);

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

        void testParse_CompleteDocument() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(!testling.parse("<iq", true));
            CPPUNIT_ASSERT(!testling.parse("<iq>", true));
            CPPUNIT_ASSERT(!testling.parse("<iq><child>foo</child>", true));
            CPPUNIT_ASSERT(testling.parse("<iq><child>foo</child></iq>", true));
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
            CPPUNIT_ASSERT_EQUAL(std::string(""), client_.events[0].attributes.getEntries()[0].getAttribute().getPrefix());
        }

        void testParse_AttributeWithNamespace() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(testling.parse(
                "<query xmlns='http://swift.im' xmlns:f='http://swift.im/f' f:attr='3'/>"));

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].attributes.getEntries().size());
            CPPUNIT_ASSERT_EQUAL(std::string("attr"), client_.events[0].attributes.getEntries()[0].getAttribute().getName());
            CPPUNIT_ASSERT_EQUAL(std::string("http://swift.im/f"), client_.events[0].attributes.getEntries()[0].getAttribute().getNamespace());
            CPPUNIT_ASSERT_EQUAL(std::string("f"), client_.events[0].attributes.getEntries()[0].getAttribute().getPrefix());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), client_.events[0].namespaces.size());
            CPPUNIT_ASSERT_EQUAL(std::string("http://swift.im"), client_.events[0].namespaces[""]);
            CPPUNIT_ASSERT_EQUAL(std::string("http://swift.im/f"), client_.events[0].namespaces["f"]);
        }

        void testParse_AttributeWithNamespaceNoPrefix() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(testling.parse(
                "<query xmlns='http://swift.im' xmlns:f='http://swift.im/f' attr='3'/>"));

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].attributes.getEntries().size());
            CPPUNIT_ASSERT_EQUAL(std::string("attr"), client_.events[0].attributes.getEntries()[0].getAttribute().getName());
            CPPUNIT_ASSERT_EQUAL(std::string(""), client_.events[0].attributes.getEntries()[0].getAttribute().getNamespace());
            CPPUNIT_ASSERT_EQUAL(std::string(""), client_.events[0].attributes.getEntries()[0].getAttribute().getPrefix());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), client_.events[0].namespaces.size());
            CPPUNIT_ASSERT_EQUAL(std::string("http://swift.im"), client_.events[0].namespaces[""]);
            CPPUNIT_ASSERT_EQUAL(std::string("http://swift.im/f"), client_.events[0].namespaces["f"]);
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

        void testParse_InternalEntity() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(!testling.parse("<!DOCTYPE foo [<!ENTITY bar \"Bar\">]><foo>&bar;</foo>"));
        }

        void testParse_UndefinedPrefix() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(testling.parse(
                "<foo:bar><bla/></foo:bar>"));

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), client_.events.size());

            CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
            CPPUNIT_ASSERT_EQUAL(std::string("foo:bar"), client_.events[0].data);
            CPPUNIT_ASSERT_EQUAL(std::string(""), client_.events[0].ns);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), client_.events[0].namespaces.size());

            CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[1].type);
            CPPUNIT_ASSERT_EQUAL(std::string("bla"), client_.events[1].data);
            CPPUNIT_ASSERT_EQUAL(std::string(""), client_.events[1].ns);

            CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[2].type);
            CPPUNIT_ASSERT_EQUAL(std::string("bla"), client_.events[2].data);
            CPPUNIT_ASSERT_EQUAL(std::string(""), client_.events[2].ns);

            CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[3].type);
            CPPUNIT_ASSERT_EQUAL(std::string("foo:bar"), client_.events[3].data);
            CPPUNIT_ASSERT_EQUAL(std::string(""), client_.events[3].ns);
        }

        void testParse_UndefinedAttributePrefix() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(testling.parse("<foo bar:baz='bla'/>"));

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), client_.events[0].attributes.getEntries().size());
            CPPUNIT_ASSERT_EQUAL(std::string("bar:baz"), client_.events[0].attributes.getEntries()[0].getAttribute().getName());
        }

        void testParse_AllowCommentsInXML() {
            ParserType testling(&client_, true);

            CPPUNIT_ASSERT(testling.parse("<message><!-- Some More Comments Testing --></message>"));

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), client_.events.size());

            CPPUNIT_ASSERT_EQUAL(Client::StartElement, client_.events[0].type);
            CPPUNIT_ASSERT_EQUAL(std::string("message"), client_.events[0].data);

            CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[1].type);
            CPPUNIT_ASSERT_EQUAL(std::string("message"), client_.events[1].data);
        }

        void testParse_DisallowCommentsInXML() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(!testling.parse("<message><!-- Some More Comments Testing --></message>"));
        }

        void testParse_Doctype() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(!testling.parse("<!DOCTYPE greeting SYSTEM \"hello.dtd\">"));
        }

        void testParse_ProcessingInstructions() {
            ParserType testling(&client_);

            CPPUNIT_ASSERT(!testling.parse("<?xml-stylesheet type=\"text/xsl\" href=\"Sample.xsl\"?>"));
        }

        void testParse_ProcessingPrefixedElement() {
            client_.testingStartElementPrefix = true;
            ParserType testling(&client_);

            CPPUNIT_ASSERT(testling.parse("<prefix:message xmlns='uri' xmlns:prefix='uriPrefix'/>"));

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), client_.events.size());

            CPPUNIT_ASSERT_EQUAL(Client::StartElementPrefix, client_.events[0].type);
            CPPUNIT_ASSERT_EQUAL(std::string("message"), client_.events[0].data);
            CPPUNIT_ASSERT_EQUAL(std::string("uriPrefix"), client_.events[0].ns);
            CPPUNIT_ASSERT_EQUAL(std::string("prefix"), client_.events[0].prefix);

            CPPUNIT_ASSERT_EQUAL(Client::EndElement, client_.events[1].type);
            CPPUNIT_ASSERT_EQUAL(std::string("message"), client_.events[1].data);
            CPPUNIT_ASSERT_EQUAL(std::string("uriPrefix"), client_.events[1].ns);
        }

        void testParse_InvalidlyEncodedInput() {
            ParserType testling(&client_);

            // The following input was generated by a fuzzer, and triggered a crash in the LibXML2 parser because
            // some types of error (buffer I/O errors, for instance) will not update the error in the parser context,
            // and the code used to rely on that error always being set if parsing failed.
            // This particular input will trick the parser into believing the encoding is UTF-16LE, which eventually will lead
            // to two invalid encodings, followed by an I/O error.  The latter will end parsing without updating the
            // error in the parsing context, which used to trigger a crash.
            testling.parse(std::string("<\0?\0\x80q type='get' id='aab9a'<<query xmlns='jabber:iq:roster'/>\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9b\x9br:i><quq:private'><storage xml s='s'\x00\x10</query></iq>", 271));
            testling.parse("<iq type='get'\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e\x9e to='ad5d8d2b25' ext='ca cs min@wonderland.t' id='aabda'><vCard xmlnr='vcard-temp'/>O/iq>");
            testling.parse("<\xff\xff\xff\x7fype:'get' to='won\x84" "erland.lit' id='aabea'><tuery xmlns='\xd8Vtp://jabber.org/p\x88ot\x8b" "col/disco#info'/>abber.org/protocol/disco#Nnfo'/></iq>");
        }

    private:
        class Client : public XMLParserClient {
            public:
                using NamespaceMap = std::unordered_map<std::string /* prefix */, std::string /* uri */>;
                enum Type { StartElement, StartElementPrefix, EndElement, CharacterData, NamespaceDefined };
                struct Event {
                    Event(
                            Type type,
                            const std::string& data,
                            const std::string& ns,
                            const std::string& prefix,
                            const AttributeMap& attributes,
                            NamespaceMap namespaces)
                                : type(type), data(data), ns(ns), prefix(prefix), attributes(attributes), namespaces(std::move(namespaces)) {}
                    Event(
                            Type type,
                            const std::string& data,
                            const std::string& ns,
                            const AttributeMap& attributes,
                            NamespaceMap namespaces = {})
                                : Event(type, data, ns, {}, attributes, std::move(namespaces)) {}
                    Event(Type type, const std::string& data, const std::string& ns = std::string())
                                : Event(type, data, ns, "", AttributeMap(), NamespaceMap()) {}

                    Type type;
                    std::string data;
                    std::string ns;
                    std::string prefix;
                    AttributeMap attributes;
                    NamespaceMap namespaces;
                };

                Client() {}

                void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) override {
                    if (testingStartElementPrefix) return;
                    events.push_back(Event(StartElement, element, ns, attributes, std::move(namespaces_)));
                }

                void handleStartElementPrefix(const std::string& prefix, const std::string& uri, const std::string& name, const AttributeMap&) override {
                    if (!testingStartElementPrefix) return;
                    events.push_back(Event(StartElementPrefix, name, uri, prefix, AttributeMap(), NamespaceMap()));
                }

                void handleEndElement(const std::string& element, const std::string& ns) override {
                    events.push_back(Event(EndElement, element, ns));
                }

                void handleCharacterData(const std::string& data) override {
                    events.push_back(Event(CharacterData, data));
                }

                void handleNamespaceDeclaration(const std::string& prefix, const std::string& uri) override {
                    namespaces_[prefix] = uri;
                }

                std::vector<Event> events;
                bool testingStartElementPrefix = false;
            private:
                NamespaceMap namespaces_;
        } client_;
};

#ifdef HAVE_EXPAT
CPPUNIT_TEST_SUITE_REGISTRATION(XMLParserTest<ExpatParser>);
#endif
#ifdef HAVE_LIBXML
CPPUNIT_TEST_SUITE_REGISTRATION(XMLParserTest<LibXMLParser>);
#endif
