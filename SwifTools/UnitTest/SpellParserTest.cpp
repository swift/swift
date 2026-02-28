/*
 * Copyright (c) 2012 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/algorithm/string.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <SwifTools/SpellParser.h>

using namespace Swift;

class SpellParserTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SpellParserTest);
    CPPUNIT_TEST(testSimpleCheckFragment);
    CPPUNIT_TEST(testWWWCheckFragment);
    CPPUNIT_TEST_SUITE_END();
    public:
        SpellParserTest() {
        }
        void tearDown() {
            position_.clear();
        }
        void testSimpleCheckFragment() {
            parser_->check("fragment test", position_);
            auto size = position_.size();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), size);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), boost::get<0>(position_.front()));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), boost::get<1>(position_.front()));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), boost::get<0>(position_.back()));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), boost::get<1>(position_.back()));
        }
        void testWWWCheckFragment() {
            parser_->check("www.link.com fragment test", position_);
            auto size = position_.size();
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), size);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), boost::get<0>(position_.front()));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), boost::get<1>(position_.front()));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), boost::get<0>(position_.back()));
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), boost::get<1>(position_.back()));
        }
    private:
        const std::unique_ptr<SpellParser> parser_ = std::unique_ptr<SpellParser>(new SpellParser());
        PositionPairList position_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpellParserTest);
