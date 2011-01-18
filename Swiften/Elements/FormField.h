/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// FIXME: We currently keep 2 values: the raw values, and the actual value.
// We should only store the raw values, and deduce the actual values from this

#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class FormField {
		public:
			typedef boost::shared_ptr<FormField> ref;

			virtual ~FormField() {}

			struct Option {
				Option(const String& label, const String& value) : label(label), value(value) {}
				String label;
				String value;
			};

			void setName(const String& name) { this->name = name; }
			const String& getName() const { return name; }

			void setLabel(const String& label) { this->label = label; }
			const String& getLabel() const { return label; }

			void setDescription(const String& description) { this->description = description; }
			const String& getDescription() const { return description; }

			void setRequired(bool required) { this->required = required; }
			bool getRequired() const { return required; }

			void addOption(const Option& option) {
				options.push_back(option);
			}

			const std::vector<Option>& getOptions() const {
				return options;
			}

			const std::vector<String> getRawValues() const {
				return rawValues;
			}

			void addRawValue(const String& value) {
				rawValues.push_back(value);
			}

		protected:
			FormField() : required(false) {}

		private:
			String name;
			String label;
			String description;
			bool required;
			std::vector<Option> options;
			std::vector<String> rawValues;
	};

	template<typename T> class GenericFormField : public FormField {
		public:
			const T& getValue() const {
				return value;
			}

			void setValue(const T& value) {
				this->value = value;
			}

		protected:
			GenericFormField() : value() {}
			GenericFormField(const T& value) : value(value) {}

		private:
			T value;
	};

#define SWIFTEN_DECLARE_FORM_FIELD(name, valueType) \
	class name##FormField : public GenericFormField< valueType > { \
		public: \
			typedef boost::shared_ptr<name##FormField> ref; \
			static ref create(const valueType& value) { \
				return ref(new name##FormField(value)); \
			} \
			static ref create() { \
				return ref(new name##FormField()); \
			} \
		private: \
			name##FormField(valueType value) : GenericFormField< valueType >(value) {} \
			name##FormField() : GenericFormField< valueType >() {} \
	};

	SWIFTEN_DECLARE_FORM_FIELD(Boolean, bool);
	SWIFTEN_DECLARE_FORM_FIELD(Fixed, String);
	SWIFTEN_DECLARE_FORM_FIELD(Hidden, String);
	SWIFTEN_DECLARE_FORM_FIELD(ListSingle, String);
	SWIFTEN_DECLARE_FORM_FIELD(TextMulti, String);
	SWIFTEN_DECLARE_FORM_FIELD(TextPrivate, String);
	SWIFTEN_DECLARE_FORM_FIELD(TextSingle, String);
	SWIFTEN_DECLARE_FORM_FIELD(JIDSingle, JID);
	SWIFTEN_DECLARE_FORM_FIELD(JIDMulti, std::vector<JID>);
	SWIFTEN_DECLARE_FORM_FIELD(ListMulti, std::vector<String>);
	SWIFTEN_DECLARE_FORM_FIELD(Untyped, std::vector<String>);
}
