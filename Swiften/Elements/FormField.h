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
#include <string>

#include <Swiften/JID/JID.h>

namespace Swift {
	class FormField {
		public:
			typedef boost::shared_ptr<FormField> ref;

			virtual ~FormField() {}

			struct Option {
				Option(const std::string& label, const std::string& value) : label(label), value(value) {}
				std::string label;
				std::string value;
			};

			void setName(const std::string& name) { this->name = name; }
			const std::string& getName() const { return name; }

			void setLabel(const std::string& label) { this->label = label; }
			const std::string& getLabel() const { return label; }

			void setDescription(const std::string& description) { this->description = description; }
			const std::string& getDescription() const { return description; }

			void setRequired(bool required) { this->required = required; }
			bool getRequired() const { return required; }

			void addOption(const Option& option) {
				options.push_back(option);
			}

			const std::vector<Option>& getOptions() const {
				return options;
			}

			const std::vector<std::string>& getRawValues() const {
				return rawValues;
			}

			void addRawValue(const std::string& value) {
				rawValues.push_back(value);
			}

		protected:
			FormField() : required(false) {}

		private:
			std::string name;
			std::string label;
			std::string description;
			bool required;
			std::vector<Option> options;
			std::vector<std::string> rawValues;
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
	SWIFTEN_DECLARE_FORM_FIELD(Fixed, std::string);
	SWIFTEN_DECLARE_FORM_FIELD(Hidden, std::string);
	SWIFTEN_DECLARE_FORM_FIELD(ListSingle, std::string);
	SWIFTEN_DECLARE_FORM_FIELD(TextMulti, std::string);
	SWIFTEN_DECLARE_FORM_FIELD(TextPrivate, std::string);
	SWIFTEN_DECLARE_FORM_FIELD(TextSingle, std::string);
	SWIFTEN_DECLARE_FORM_FIELD(JIDSingle, JID);
	SWIFTEN_DECLARE_FORM_FIELD(JIDMulti, std::vector<JID>);
	SWIFTEN_DECLARE_FORM_FIELD(ListMulti, std::vector<std::string>);
}
