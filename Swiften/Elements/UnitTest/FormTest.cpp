/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Form.h>

using namespace Swift;

class FormTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(FormTest);
		CPPUNIT_TEST(testGetFormType);
		CPPUNIT_TEST(testGetFormType_InvalidFormType);
		CPPUNIT_TEST(testGetFormType_NoFormType);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetFormType() {
			Form form;

			form.addField(FixedFormField::create("Foo"));

			FormField::ref field = HiddenFormField::create("jabber:bot");
			field->setName("FORM_TYPE");
			form.addField(field);

			form.addField(FixedFormField::create("Bar"));

			CPPUNIT_ASSERT_EQUAL(std::string("jabber:bot"), form.getFormType());
		}

		void testGetFormType_InvalidFormType() {
			Form form;

			FormField::ref field = FixedFormField::create("jabber:bot");
			field->setName("FORM_TYPE");
			form.addField(field);

			CPPUNIT_ASSERT_EQUAL(std::string(""), form.getFormType());
		}

		void testGetFormType_NoFormType() {
			Form form;

			form.addField(FixedFormField::create("Foo"));

			CPPUNIT_ASSERT_EQUAL(std::string(""), form.getFormType());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(FormTest);
