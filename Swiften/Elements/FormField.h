/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API FormField {
        public:
            typedef std::shared_ptr<FormField> ref;

            enum Type {
                UnknownType,
                BooleanType,
                FixedType,
                HiddenType,
                ListSingleType,
                TextMultiType,
                TextPrivateType,
                TextSingleType,
                JIDSingleType,
                JIDMultiType,
                ListMultiType
            };

            FormField(Type type = UnknownType) : type(type), required(false) {}
            FormField(Type type, const std::string& value) : type(type), required(false) {
                addValue(value);
            }
            virtual ~FormField();

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

            void clearOptions() {
                options.clear();
            }

            const std::vector<std::string>& getValues() const {
                return values;
            }

            void addValue(const std::string& value) {
                values.push_back(value);
            }

            Type getType() const {
                return type;
            }

            void setType(Type type) {
                this->type = type;
            }

            // Type specific

            bool getBoolValue() const {
                assert(type == BooleanType || type == UnknownType);
                if (values.empty()) {
                    return false;
                }
                return values[0] == "true" || values[0] == "1";
            }

            void setBoolValue(bool b);

            JID getJIDSingleValue() const {
                assert(type == JIDSingleType || type == UnknownType);
                return values.empty() ? JID() : JID(values[0]);
            }

            JID getJIDMultiValue(size_t index) const {
                assert(type == JIDMultiType || type == UnknownType);
                return values.empty() ? JID() : JID(values[index]);
            }

            std::string getTextPrivateValue() const {
                assert(type == TextPrivateType || type == UnknownType);
                return values.empty() ? "" : values[0];
            }

            std::string getFixedValue() const {
                assert(type == FixedType || type == UnknownType);
                return values.empty() ? "" : values[0];
            }

            std::string getTextSingleValue() const {
                assert(type == TextSingleType || type == UnknownType);
                return values.empty() ? "" : values[0];
            }

            std::string getTextMultiValue() const;
            void setTextMultiValue(const std::string& value);

        protected:

        private:
            Type type;
            std::string name;
            std::string label;
            std::string description;
            bool required;
            std::vector<Option> options;
            std::vector<std::string> values;
    };

}
