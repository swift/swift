/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Form.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class FormParser : public GenericPayloadParser<Form> {
		public:
			FormParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			class FieldParseHelper {
				public:
					virtual ~FieldParseHelper() {}
					virtual void addValue(const String&) = 0;
					virtual boost::shared_ptr<FormField> getField() const {
						return field;
					}
				protected:
					boost::shared_ptr<FormField> field;
			};
			class BoolFieldParseHelper : public FieldParseHelper {
					virtual void addValue(const String& s) {
						boost::dynamic_pointer_cast< GenericFormField<bool> >(getField())->setValue(s == "1" || s == "true");
						getField()->addRawValue(s);
					}
			};
			class StringFieldParseHelper : public FieldParseHelper {
					virtual void addValue(const String& s) {
						boost::shared_ptr<GenericFormField<String> > field = boost::dynamic_pointer_cast< GenericFormField<String> >(getField());
						if (field->getValue().isEmpty()) {
							field->setValue(s);
						}
						else {
							field->setValue(field->getValue() + "\n" + s);
						}
						getField()->addRawValue(s);
					}
			};
			class JIDFieldParseHelper : public FieldParseHelper {
					virtual void addValue(const String& s) {
						boost::dynamic_pointer_cast< GenericFormField<JID> >(getField())->setValue(JID(s));
					}
			};
			class StringListFieldParseHelper : public FieldParseHelper {
					virtual void addValue(const String& s) {
						// FIXME: Inefficient, but too much hassle to do efficiently
						boost::shared_ptr<GenericFormField< std::vector<String> > > field = boost::dynamic_pointer_cast< GenericFormField<std::vector<String > > >(getField());
						std::vector<String> l = field->getValue();
						l.push_back(s);
						field->setValue(l);
						getField()->addRawValue(s);
					}
			};
			class JIDListFieldParseHelper : public FieldParseHelper {
					virtual void addValue(const String& s) {
						// FIXME: Inefficient, but too much hassle to do efficiently
						boost::shared_ptr< GenericFormField< std::vector<JID> > > field = boost::dynamic_pointer_cast< GenericFormField<std::vector<JID > > >(getField());
						std::vector<JID> l = field->getValue();
						l.push_back(JID(s));
						field->setValue(l);
						getField()->addRawValue(s);
					}
			};
			
#define SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(name, baseParser) \
	class name##FormFieldParseHelper : public baseParser##FieldParseHelper { \
		public: \
			typedef boost::shared_ptr<name##FormFieldParseHelper> ref; \
			static ref create() { \
				return ref(new name##FormFieldParseHelper()); \
			} \
		private: \
			name##FormFieldParseHelper() : baseParser##FieldParseHelper() { \
				field = name##FormField::create(); \
			} \
	};

			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(Boolean, Bool);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(Fixed, String);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(Hidden, String);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(ListSingle, String);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(TextMulti, String);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(TextPrivate, String);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(TextSingle, String);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(JIDSingle, JID);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(JIDMulti, JIDList);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(ListMulti, StringList);
			SWIFTEN_DECLARE_FORM_FIELD_PARSE_HELPER(Untyped, StringList);

			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				FieldLevel = 2
			};
			int level_;
			String currentText_;
			String currentOptionLabel_;
			boost::shared_ptr<FieldParseHelper> currentFieldParseHelper_;
	};
}
