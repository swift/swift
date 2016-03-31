/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/foreach.h>

#include <Swift/Controllers/ContactSuggester.h>

using namespace Swift;

class ContactSuggesterTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ContactSuggesterTest);
    CPPUNIT_TEST(equalityTest);
    CPPUNIT_TEST(lexicographicalSortTest);
    CPPUNIT_TEST(sortTest);
    CPPUNIT_TEST_SUITE_END();

public:

    std::vector<std::string> wordList() {
        const std::string words[] = {
            "abc",
            "ab",
            "bc",
            "d"
        };

        return std::vector<std::string>(words, words+sizeof(words)/sizeof(*words));
    }

    std::vector<StatusShow::Type> statusList() {
        StatusShow::Type types[] = {
            StatusShow::Online,
            StatusShow::Away,
            StatusShow::FFC,
            StatusShow::XA,
            StatusShow::DND,
            StatusShow::None
        };

        return std::vector<StatusShow::Type>(types, types+sizeof(types)/sizeof(*types));
    }

    std::vector<Contact::ref> contactList() {
        std::vector<Contact::ref> contacts;
        std::vector<std::string> words = wordList();
        std::vector<StatusShow::Type> statuses = statusList();
        foreach (const std::string& name, words) {
            foreach (const std::string& jid, words) {
                foreach (const StatusShow::Type& status, statuses) {
                    contacts.push_back(boost::make_shared<Contact>(name, jid, status, ""));
                }
            }
        }
        return contacts;
    }

    /* a = a */
    bool isReflexive(const boost::function<bool (const Contact::ref&, const Contact::ref&)>& comparitor) {
        std::vector<Contact::ref> contacts = contactList();
        foreach (const Contact::ref& a, contacts) {
            if (!comparitor(a, a)) {
                return false;
            }
        }
        return true;
    }

    /* a = b -> b = a */
    bool isSymmetric(const boost::function<bool (const Contact::ref&, const Contact::ref&)>& comparitor) {
        std::vector<Contact::ref> contacts = contactList();
        foreach (const Contact::ref& a, contacts) {
            foreach (const Contact::ref& b, contacts) {
                if (comparitor(a, b)) {
                    if (!comparitor(b, a)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /* a = b && b = c -> a = c */
    bool isTransitive(const boost::function<bool (const Contact::ref&, const Contact::ref&)>& comparitor) {
        std::vector<Contact::ref> contacts = contactList();
        foreach (const Contact::ref& a, contacts) {
            foreach (const Contact::ref& b, contacts) {
                foreach (const Contact::ref& c, contacts) {
                    if (comparitor(a, b) && comparitor(b, c)) {
                        if (!comparitor(a, c)) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    void equalityTest() {
        CPPUNIT_ASSERT(isReflexive(Contact::equalityPredicate));
        CPPUNIT_ASSERT(isSymmetric(Contact::equalityPredicate));
        CPPUNIT_ASSERT(isTransitive(Contact::equalityPredicate));
    }

    void lexicographicalSortTest() {
        CPPUNIT_ASSERT(isTransitive(Contact::lexicographicalSortPredicate));
    }

    void sortTest() {
        std::vector<std::string> words = wordList();
        foreach (const std::string& word, words) {
            CPPUNIT_ASSERT(isTransitive(boost::bind(Contact::sortPredicate, _1, _2, word)));
        }
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(ContactSuggesterTest);
