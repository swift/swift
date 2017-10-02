/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Platform.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadParserTester.h>

using namespace Swift;


#if !SWIFTEN_STATIC && defined(SWIFTEN_PLATFORM_WINDOWS)
// This base class of a class used in this file is already exported, so need to
// explicitly import it.
template class __declspec(dllimport) Swift::GenericPayloadParser<RawXMLPayload>;
#endif

class GenericPayloadTreeParserTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(GenericPayloadTreeParserTest);
        CPPUNIT_TEST(testTree);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testTree() {
            MyParser testling;

            std::string data = "<topLevel xmlns='urn:test:top'><firstLevelInheritedEmpty/><firstLevelInherited><secondLevelMultiChildren num='1'/><secondLevelMultiChildren num='2'/></firstLevelInherited><firstLevelNS xmlns='urn:test:first'/></topLevel>";

            PayloadParserTester tester(&testling);
            tester.parse(data);

            ParserElement::ref tree = testling.tree;

            CPPUNIT_ASSERT_EQUAL(std::string("topLevel"), tree->getName());
            CPPUNIT_ASSERT_EQUAL(std::string("urn:test:top"), tree->getNamespace());
            CPPUNIT_ASSERT(tree->getChild("firstLevelInheritedEmpty", "urn:test:top"));
            CPPUNIT_ASSERT(!*tree->getChild("firstLevelInheritedEmpty", ""));
            CPPUNIT_ASSERT(tree->getChild("firstLevelInherited", "urn:test:top"));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree->getChild("firstLevelInherited", "urn:test:top")->getChildren("secondLevelMultiChildren", "urn:test:top").size());
            CPPUNIT_ASSERT_EQUAL(std::string("1"), tree->getChild("firstLevelInherited", "urn:test:top")->getChildren("secondLevelMultiChildren", "urn:test:top")[0]->getAttributes().getAttribute("num"));
            CPPUNIT_ASSERT_EQUAL(std::string("2"), tree->getChild("firstLevelInherited", "urn:test:top")->getChildren("secondLevelMultiChildren", "urn:test:top")[1]->getAttributes().getAttribute("num"));
            CPPUNIT_ASSERT(tree->getChild("firstLevelNS", "urn:test:first"));
        }

    private:


        class MyParser : public GenericPayloadTreeParser<RawXMLPayload>
        {
            public:
                virtual ~MyParser() {}
                virtual void handleTree(ParserElement::ref root) {
                    tree = root;
                }
                ParserElement::ref tree;
        };

};

CPPUNIT_TEST_SUITE_REGISTRATION(GenericPayloadTreeParserTest);
