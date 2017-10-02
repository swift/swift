/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/FormConvertor.h>

#include <memory>
#include <sstream>

#include <boost/assign/list_of.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/Lua/Check.h>
#include <Sluift/Lua/Value.h>

using namespace Swift;

namespace {
    int formIndex(lua_State* L) {
        lua_getfield(L, 1, "fields");
        if (lua_type(L, -1) != LUA_TTABLE) {
            return 0;
        }
        int index = Lua::absoluteOffset(L, -1);
        lua_pushnil(L);
        for (lua_pushnil(L); lua_next(L, index) != 0; ) {
            lua_getfield(L, -1, "name");
            if (lua_equal(L, -1, 2)) {
                lua_pop(L, 1);
                return 1;
            }
            lua_pop(L, 2);
        }
        return 0;
    }

    int formNewIndex(lua_State* L) {
        lua_getfield(L, 1, "fields");
        bool foundField = false;
        if (lua_type(L, -1) == LUA_TTABLE) {
            for (lua_pushnil(L); lua_next(L, -2) != 0; ) {
                lua_getfield(L, -1, "name");
                if (lua_equal(L, -1, 2)) {
                    lua_pushvalue(L, 3);
                    lua_setfield(L, -3, "value");
                    foundField = true;
                    lua_pop(L, 3);
                    break;
                }
                lua_pop(L, 2);
            }
        }
        lua_pop(L, 1);

        if (!foundField) {
            lua_pushvalue(L, 2);
            lua_pushvalue(L, 3);
            lua_rawset(L, 1);
        }
        return 0;
    }

    Lua::Table convertFieldToLua(std::shared_ptr<FormField> field) {
        Lua::Table luaField = boost::assign::map_list_of("name", Lua::valueRef(field->getName()));
        std::string type;
        switch (field->getType()) {
            case FormField::UnknownType: type = ""; break;
            case FormField::BooleanType: type = "boolean"; break;
            case FormField::FixedType: type = "fixed"; break;
            case FormField::HiddenType: type = "hidden"; break;
            case FormField::ListSingleType: type = "list-single"; break;
            case FormField::TextMultiType: type = "text-multi"; break;
            case FormField::TextPrivateType: type = "text-private"; break;
            case FormField::TextSingleType: type = "text-single"; break;
            case FormField::JIDSingleType: type = "jid-single"; break;
            case FormField::JIDMultiType: type = "jid-multi"; break;
            case FormField::ListMultiType: type = "list-multi"; break;
        }
        if (!type.empty()) {
            luaField["type"] = Lua::valueRef(type);
        }
        if (!field->getLabel().empty()) {
            luaField["label"] = Lua::valueRef(field->getLabel());
        }
        if (field->getRequired()) {
            luaField["required"] = Lua::boolRef(field->getRequired());
        }
        if (!field->getDescription().empty()) {
            luaField["description"] = Lua::valueRef(field->getDescription());
        }
        if (field->getType() == FormField::BooleanType) {
            luaField["value"] = Lua::boolRef(field->getBoolValue());
        }
        else if (field->getValues().size() > 1) {
            luaField["value"] = Lua::valueRef(Lua::Array(field->getValues().begin(), field->getValues().end()));
        }
        else if (field->getValues().size() == 1) {
            luaField["value"] = Lua::valueRef(field->getValues()[0]);
        }
        if (!field->getOptions().empty()) {
            Lua::Array options;
            for (const auto& option : field->getOptions()) {
                Lua::Table luaOption = boost::assign::map_list_of
                    ("label", Lua::valueRef(option.label))
                    ("value", Lua::valueRef(option.value));
                options.push_back(luaOption);
            }
            luaField["options"] = valueRef(options);
        }
        return luaField;
    }

    Lua::Array convertFieldListToLua(const std::vector< std::shared_ptr<FormField> >& fieldList) {
        Lua::Array fields;
        for (auto&& field : fieldList) {
            fields.push_back(convertFieldToLua(field));
        }
        return fields;
    }


    std::shared_ptr<FormField> convertFieldFromLua(lua_State* L) {
        std::shared_ptr<FormField> result = std::make_shared<FormField>();
        FormField::Type fieldType = FormField::UnknownType;
        boost::optional<std::string> type = Lua::getStringField(L, -1, "type");
        if (type) {
            if (*type == "boolean") {
                fieldType = FormField::BooleanType;
            }
            if (*type == "fixed") {
                fieldType = FormField::FixedType;
            }
            if (*type == "hidden") {
                fieldType = FormField::HiddenType;
            }
            if (*type == "list-single") {
                fieldType = FormField::ListSingleType;
            }
            if (*type == "text-multi") {
                fieldType = FormField::TextMultiType;
            }
            if (*type == "text-private") {
                fieldType = FormField::TextPrivateType;
            }
            if (*type == "text-single") {
                fieldType = FormField::TextSingleType;
            }
            if (*type == "jid-single") {
                fieldType = FormField::JIDSingleType;
            }
            if (*type == "jid-multi") {
                fieldType = FormField::JIDMultiType;
            }
            if (*type == "list-multi") {
                fieldType = FormField::ListMultiType;
            }
        }
        result->setType(fieldType);
        if (boost::optional<std::string> name = Lua::getStringField(L, -1, "name")) {
            result->setName(*name);
        }
        if (boost::optional<std::string> description = Lua::getStringField(L, -1, "description")) {
            result->setDescription(*description);
        }
        if (boost::optional<std::string> label = Lua::getStringField(L, -1, "label")) {
            result->setLabel(*label);
        }
        if (boost::optional<bool> required = Lua::getBooleanField(L, -1, "required")) {
            result->setRequired(*required);
        }
        if (boost::optional<std::string> value = Lua::getStringField(L, -1, "value")) {
            result->addValue(*value);
        }
        else if (boost::optional<bool> value = Lua::getBooleanField(L, -1, "value")) {
            result->setBoolValue(*value);
        }
        else {
            lua_getfield(L, -1, "value");
            if (lua_istable(L, -1)) {
                for (lua_pushnil(L); lua_next(L, -2); ) {
                    if (lua_isstring(L, -1)) {
                        result->addValue(lua_tostring(L, -1));
                    }
                    lua_pop(L, 1);
                }
            }
            lua_pop(L, 1);
        }
        lua_getfield(L, -1, "options");
        if (lua_istable(L, -1)) {
            for (lua_pushnil(L); lua_next(L, -2); ) {
                if (lua_istable(L, -1)) {
                    FormField::Option option("", "");
                    if (boost::optional<std::string> value = Lua::getStringField(L, -1, "value")) {
                        option.value = *value;
                    }
                    if (boost::optional<std::string> label = Lua::getStringField(L, -1, "label")) {
                        option.label = *label;
                    }
                    result->addOption(option);
                }
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);
        return result;
    }

    std::vector< std::shared_ptr<FormField> > convertFieldListFromLua(lua_State* L) {
        std::vector< std::shared_ptr<FormField> > result;
        for (lua_pushnil(L); lua_next(L, -2);) {
            result.push_back(convertFieldFromLua(L));
            lua_pop(L, 1);
        }
        return result;
    }

    std::shared_ptr<Form> convertFormFromLua(lua_State* L) {
        std::shared_ptr<Form> result = std::make_shared<Form>();
        if (boost::optional<std::string> title = Lua::getStringField(L, -1, "title")) {
            result->setTitle(*title);
        }
        if (boost::optional<std::string> instructions = Lua::getStringField(L, -1, "instructions")) {
            result->setInstructions(*instructions);
        }
        if (boost::optional<std::string> type = Lua::getStringField(L, -1, "type")) {
            Form::Type formType = Form::FormType;
            if (*type == "submit") {
                formType = Form::SubmitType;
            }
            else if (*type == "cancel") {
                formType = Form::CancelType;
            }
            else if (*type == "result") {
                formType = Form::ResultType;
            }
            result->setType(formType);
        }

        lua_getfield(L, -1, "fields");
        if (lua_istable(L, -1)) {
            for (auto&& formField : convertFieldListFromLua(L)) {
                result->addField(formField);
            }
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "reported_fields");
        if (lua_istable(L, -1)) {
            for (auto&& formField : convertFieldListFromLua(L)) {
                result->addReportedField(formField);
            }
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "items");
        if (lua_istable(L, -1)) {
            for (lua_pushnil(L); lua_next(L, -2);) {
                result->addItem(convertFieldListFromLua(L));
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);

        return result;
    }

    void convertFormToLua(lua_State* L, std::shared_ptr<Form> payload) {
        std::string type;
        switch (payload->getType()) {
            case Form::FormType: type = "form"; break;
            case Form::SubmitType: type = "submit"; break;
            case Form::CancelType: type = "cancel"; break;
            case Form::ResultType: type = "result"; break;
        }

        Lua::Table result = boost::assign::map_list_of("type", Lua::valueRef(type));
        if (!payload->getTitle().empty()) {
            result["title"] = Lua::valueRef(payload->getTitle());
        }
        if (!payload->getInstructions().empty()) {
            result["instructions"] = Lua::valueRef(payload->getInstructions());
        }
        result["fields"] = valueRef(convertFieldListToLua(payload->getFields()));
        if (!payload->getReportedFields().empty()) {
            result["reported_fields"] = valueRef(convertFieldListToLua(payload->getReportedFields()));
        }

        if (!payload->getItems().empty()) {
            Lua::Array luaItems;
            for (const auto& item : payload->getItems()) {
                if (!item.empty()) {
                    luaItems.push_back(convertFieldListToLua(item));
                }
            }
            result["items"] = valueRef(luaItems);
        }

        Lua::pushValue(L, result);

        lua_newtable(L);
        lua_pushcfunction(L, formIndex);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, formNewIndex);
        lua_setfield(L, -2, "__newindex");
        lua_setmetatable(L, -2);
    }

    int createSubmission(lua_State* L) {
        std::shared_ptr<Form> form = convertFormFromLua(L);

        // Remove all redundant elements
        form->setInstructions("");
        form->setTitle("");
        form->clearItems();
        form->clearReportedFields();
        std::vector< std::shared_ptr<FormField> > fields(form->getFields());
        form->clearFields();
        for (auto&& field : fields) {
            if (field->getType() == FormField::FixedType) {
                continue;
            }
            field->clearOptions();
            field->setLabel("");
            field->setType(FormField::UnknownType);
            field->setDescription("");
            form->addField(field);
        }
        form->setType(Form::SubmitType);

        // Convert back
        convertFormToLua(L, form);
        Lua::registerTableToString(L, -1);

        return 1;
    }
}

FormConvertor::FormConvertor() : GenericLuaElementConvertor<Form>("form") {
}

FormConvertor::~FormConvertor() {
}

std::shared_ptr<Form> FormConvertor::doConvertFromLua(lua_State* L) {
    return convertFormFromLua(L);
}

void FormConvertor::doConvertToLua(lua_State* L, std::shared_ptr<Form> payload) {
    convertFormToLua(L, payload);

    lua_pushstring(L, "create_submission");
    lua_pushcfunction(L, createSubmission);
    lua_rawset(L, -3);
}
