/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/UserLocationConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Swiften/Base/DateTime.h>

using namespace Swift;

UserLocationConvertor::UserLocationConvertor() :
        GenericLuaElementConvertor<UserLocation>("user_location") {
}

UserLocationConvertor::~UserLocationConvertor() {
}

std::shared_ptr<UserLocation> UserLocationConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<UserLocation> result = std::make_shared<UserLocation>();
    lua_getfield(L, -1, "area");
    if (lua_isstring(L, -1)) {
        result->setArea(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "altitude");
    if (lua_isnumber(L, -1)) {
        result->setAltitude(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "locality");
    if (lua_isstring(L, -1)) {
        result->setLocality(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "latitude");
    if (lua_isnumber(L, -1)) {
        result->setLatitude(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "accuracy");
    if (lua_isnumber(L, -1)) {
        result->setAccuracy(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "description");
    if (lua_isstring(L, -1)) {
        result->setDescription(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "country_code");
    if (lua_isstring(L, -1)) {
        result->setCountryCode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "timestamp");
    if (lua_isstring(L, -1)) {
        result->setTimestamp(stringToDateTime(std::string(lua_tostring(L, -1))));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "floor");
    if (lua_isstring(L, -1)) {
        result->setFloor(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "building");
    if (lua_isstring(L, -1)) {
        result->setBuilding(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "room");
    if (lua_isstring(L, -1)) {
        result->setRoom(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "country");
    if (lua_isstring(L, -1)) {
        result->setCountry(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "region");
    if (lua_isstring(L, -1)) {
        result->setRegion(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "uri");
    if (lua_isstring(L, -1)) {
        result->setURI(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "longitude");
    if (lua_isnumber(L, -1)) {
        result->setLongitude(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "error");
    if (lua_isnumber(L, -1)) {
        result->setError(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "postal_code");
    if (lua_isstring(L, -1)) {
        result->setPostalCode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "bearing");
    if (lua_isnumber(L, -1)) {
        result->setBearing(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "text");
    if (lua_isstring(L, -1)) {
        result->setText(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "datum");
    if (lua_isstring(L, -1)) {
        result->setDatum(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "street");
    if (lua_isstring(L, -1)) {
        result->setStreet(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "speed");
    if (lua_isnumber(L, -1)) {
        result->setSpeed(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    return result;
}

void UserLocationConvertor::doConvertToLua(lua_State* L, std::shared_ptr<UserLocation> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getArea()) {
        lua_pushstring(L, (*payload->getArea()).c_str());
        lua_setfield(L, -2, "area");
    }
    if (payload->getAltitude()) {
        lua_pushnumber(L, (*payload->getAltitude()));
        lua_setfield(L, -2, "altitude");
    }
    if (payload->getLocality()) {
        lua_pushstring(L, (*payload->getLocality()).c_str());
        lua_setfield(L, -2, "locality");
    }
    if (payload->getLatitude()) {
        lua_pushnumber(L, (*payload->getLatitude()));
        lua_setfield(L, -2, "latitude");
    }
    if (payload->getAccuracy()) {
        lua_pushnumber(L, (*payload->getAccuracy()));
        lua_setfield(L, -2, "accuracy");
    }
    if (payload->getDescription()) {
        lua_pushstring(L, (*payload->getDescription()).c_str());
        lua_setfield(L, -2, "description");
    }
    if (payload->getCountryCode()) {
        lua_pushstring(L, (*payload->getCountryCode()).c_str());
        lua_setfield(L, -2, "country_code");
    }
    if (payload->getTimestamp()) {
    lua_pushstring(L, dateTimeToString((*payload->getTimestamp())).c_str());
        lua_setfield(L, -2, "timestamp");
    }
    if (payload->getFloor()) {
        lua_pushstring(L, (*payload->getFloor()).c_str());
        lua_setfield(L, -2, "floor");
    }
    if (payload->getBuilding()) {
        lua_pushstring(L, (*payload->getBuilding()).c_str());
        lua_setfield(L, -2, "building");
    }
    if (payload->getRoom()) {
        lua_pushstring(L, (*payload->getRoom()).c_str());
        lua_setfield(L, -2, "room");
    }
    if (payload->getCountry()) {
        lua_pushstring(L, (*payload->getCountry()).c_str());
        lua_setfield(L, -2, "country");
    }
    if (payload->getRegion()) {
        lua_pushstring(L, (*payload->getRegion()).c_str());
        lua_setfield(L, -2, "region");
    }
    if (payload->getURI()) {
        lua_pushstring(L, (*payload->getURI()).c_str());
        lua_setfield(L, -2, "uri");
    }
    if (payload->getLongitude()) {
        lua_pushnumber(L, (*payload->getLongitude()));
        lua_setfield(L, -2, "longitude");
    }
    if (payload->getError()) {
        lua_pushnumber(L, (*payload->getError()));
        lua_setfield(L, -2, "error");
    }
    if (payload->getPostalCode()) {
        lua_pushstring(L, (*payload->getPostalCode()).c_str());
        lua_setfield(L, -2, "postal_code");
    }
    if (payload->getBearing()) {
        lua_pushnumber(L, (*payload->getBearing()));
        lua_setfield(L, -2, "bearing");
    }
    if (payload->getText()) {
        lua_pushstring(L, (*payload->getText()).c_str());
        lua_setfield(L, -2, "text");
    }
    if (payload->getDatum()) {
        lua_pushstring(L, (*payload->getDatum()).c_str());
        lua_setfield(L, -2, "datum");
    }
    if (payload->getStreet()) {
        lua_pushstring(L, (*payload->getStreet()).c_str());
        lua_setfield(L, -2, "street");
    }
    if (payload->getSpeed()) {
        lua_pushnumber(L, (*payload->getSpeed()));
        lua_setfield(L, -2, "speed");
    }
}

boost::optional<LuaElementConvertor::Documentation> UserLocationConvertor::getDocumentation() const {
    return Documentation(
        "UserLocation",
        "This table has the following fields:\n\n"
        "- `area`: string (Optional)\n"
        "- `altitude`: @{float} (Optional)\n"
        "- `locality`: string (Optional)\n"
        "- `latitude`: @{float} (Optional)\n"
        "- `accuracy`: @{float} (Optional)\n"
        "- `description`: string (Optional)\n"
        "- `country_code`: string (Optional)\n"
        "- `timestamp`: datetime (string) (Optional)\n"
        "- `floor`: string (Optional)\n"
        "- `building`: string (Optional)\n"
        "- `room`: string (Optional)\n"
        "- `country`: string (Optional)\n"
        "- `region`: string (Optional)\n"
        "- `uri`: string (Optional)\n"
        "- `longitude`: @{float} (Optional)\n"
        "- `error`: @{float} (Optional)\n"
        "- `postal_code`: string (Optional)\n"
        "- `bearing`: @{float} (Optional)\n"
        "- `text`: string (Optional)\n"
        "- `datum`: string (Optional)\n"
        "- `street`: string (Optional)\n"
        "- `speed`: @{float} (Optional)\n"
    );
}
