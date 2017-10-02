/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/VCardConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/DateTime.h>

#include <Sluift/Lua/LuaUtils.h>

using namespace Swift;

VCardConvertor::VCardConvertor() : GenericLuaElementConvertor<VCard>("vcard") {
}

VCardConvertor::~VCardConvertor() {
}

std::shared_ptr<VCard> VCardConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<VCard> result = std::make_shared<VCard>();
    lua_getfield(L, -1, "fullname");
    if (lua_isstring(L, -1)) {
        result->setFullName(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "familyname");
    if (lua_isstring(L, -1)) {
        result->setFamilyName(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "givenname");
    if (lua_isstring(L, -1)) {
        result->setGivenName(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "middlename");
    if (lua_isstring(L, -1)) {
        result->setMiddleName(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "prefix");
    if (lua_isstring(L, -1)) {
        result->setPrefix(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "suffix");
    if (lua_isstring(L, -1)) {
        result->setSuffix(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "nick");
    if (lua_isstring(L, -1)) {
        result->setNickname(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "description");
    if (lua_isstring(L, -1)) {
        result->setDescription(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "photo");
    if (lua_isstring(L, -1)) {
        size_t len;
        const char* data = lua_tolstring(L, -1, &len);
        result->setPhoto(createByteArray(data, len));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "phototype");
    if (lua_isstring(L, -1)) {
        result->setPhotoType(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "birthday");
    if (lua_isstring(L, -1)) {
        result->setBirthday(stringToDateTime(std::string(lua_tostring(L, -1))));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "email");
    if (lua_istable(L, -1)) {
         for (lua_pushnil(L); lua_next(L, -2); ) {
            VCard::EMailAddress emailAddress;
            emailAddress.address = Lua::getStringField(L, -1, "address").get_value_or("");
            if (boost::optional<bool> home = Lua::getBooleanField(L, -1, "home")) {
                emailAddress.isHome = *home;
            }
            if (boost::optional<bool> work = Lua::getBooleanField(L, -1, "work")) {
                emailAddress.isWork = *work;
            }
            if (boost::optional<bool> internet = Lua::getBooleanField(L, -1, "internet")) {
                emailAddress.isInternet = *internet;
            }
            if (boost::optional<bool> preferred = Lua::getBooleanField(L, -1, "preferred")) {
                emailAddress.isPreferred = *preferred;
            }
            if (boost::optional<bool> x400 = Lua::getBooleanField(L, -1, "x400")) {
                emailAddress.isX400 = *x400;
            }
            result->addEMailAddress(emailAddress);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "telephone");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); ) {
            VCard::Telephone telephone;
            telephone.number = Lua::getStringField(L, -1, "number").get_value_or("");
            if (boost::optional<bool> home = Lua::getBooleanField(L, -1, "home")) {
                telephone.isHome = *home;
            }
            if (boost::optional<bool> work = Lua::getBooleanField(L, -1, "work")) {
                telephone.isWork = *work;
            }
            if (boost::optional<bool> voice = Lua::getBooleanField(L, -1, "voice")) {
                telephone.isVoice = *voice;
            }
            if (boost::optional<bool> fax = Lua::getBooleanField(L, -1, "fax")) {
                telephone.isFax = *fax;
            }
            if (boost::optional<bool> pager = Lua::getBooleanField(L, -1, "pager")) {
                telephone.isPager = *pager;
            }
            if (boost::optional<bool> msg = Lua::getBooleanField(L, -1, "msg")) {
                telephone.isMSG = *msg;
            }
            if (boost::optional<bool> cell = Lua::getBooleanField(L, -1, "cell")) {
                telephone.isCell = *cell;
            }
            if (boost::optional<bool> video = Lua::getBooleanField(L, -1, "video")) {
                telephone.isVideo = *video;
            }
            if (boost::optional<bool> bbs = Lua::getBooleanField(L, -1, "bbs")) {
                telephone.isBBS = *bbs;
            }
            if (boost::optional<bool> modem = Lua::getBooleanField(L, -1, "modem")) {
                telephone.isModem = *modem;
            }
            if (boost::optional<bool> isdn = Lua::getBooleanField(L, -1, "isdn")) {
                telephone.isISDN = *isdn;
            }
            if (boost::optional<bool> pcs = Lua::getBooleanField(L, -1, "pcs")) {
                telephone.isPCS = *pcs;
            }
            if (boost::optional<bool> preferred = Lua::getBooleanField(L, -1, "preferred")) {
                telephone.isPreferred = *preferred;
            }
            result->addTelephone(telephone);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "address");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); ) {
            VCard::Address address;
            address.poBox = Lua::getStringField(L, -1, "pobox").get_value_or("");
            address.addressExtension = Lua::getStringField(L, -1, "extension").get_value_or("");
            address.street = Lua::getStringField(L, -1, "street").get_value_or("");
            address.locality = Lua::getStringField(L, -1, "locality").get_value_or("");
            address.region = Lua::getStringField(L, -1, "region").get_value_or("");
            address.postalCode = Lua::getStringField(L, -1, "postalcode").get_value_or("");
            address.country = Lua::getStringField(L, -1, "country").get_value_or("");
            if (boost::optional<bool> home = Lua::getBooleanField(L, -1, "home")) {
                address.isHome = *home;
            }
            if (boost::optional<bool> work = Lua::getBooleanField(L, -1, "work")) {
                address.isWork = *work;
            }
            if (boost::optional<bool> postal = Lua::getBooleanField(L, -1, "postal")) {
                address.isPostal = *postal;
            }
            if (boost::optional<bool> parcel = Lua::getBooleanField(L, -1, "parcel")) {
                address.isParcel = *parcel;
            }
            if (boost::optional<bool> preferred = Lua::getBooleanField(L, -1, "preferred")) {
                address.isPreferred = *preferred;
            }
            if (boost::optional<bool> domestic = Lua::getBooleanField(L, -1, "domestic")) {
                if (*domestic) {
                    address.deliveryType = VCard::DomesticDelivery;
                }
            }
            if (boost::optional<bool> international = Lua::getBooleanField(L, -1, "international")) {
                if (*international) {
                    address.deliveryType = VCard::InternationalDelivery;
                }
            }
            result->addAddress(address);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "addresslabel");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); ) {
            VCard::AddressLabel addresslabel;
            lua_getfield(L, -1, "lines");
            if (lua_istable(L, -1)) {
                for (lua_pushnil(L); lua_next(L, -2); ) {
                    if (lua_isstring(L, -1)) {
                        addresslabel.lines.push_back(lua_tostring(L, -1));
                    }
                        lua_pop(L, 1);
                    }
            }
            lua_pop(L, 1);
            if (boost::optional<bool> home = Lua::getBooleanField(L, -1, "home")) {
                addresslabel.isHome = *home;
            }
            if (boost::optional<bool> work = Lua::getBooleanField(L, -1, "work")) {
                addresslabel.isWork = *work;
            }
            if (boost::optional<bool> postal = Lua::getBooleanField(L, -1, "postal")) {
                addresslabel.isPostal = *postal;
            }
            if (boost::optional<bool> parcel = Lua::getBooleanField(L, -1, "parcel")) {
                addresslabel.isParcel = *parcel;
            }
            if (boost::optional<bool> preferred = Lua::getBooleanField(L, -1, "preferred")) {
                addresslabel.isPreferred = *preferred;
            }
            if (boost::optional<bool> domestic = Lua::getBooleanField(L, -1, "domestic")) {
                if (*domestic) {
                    addresslabel.deliveryType = VCard::DomesticDelivery;
                }
            }
            if (boost::optional<bool> international = Lua::getBooleanField(L, -1, "international")) {
                if (*international) {
                    addresslabel.deliveryType = VCard::InternationalDelivery;
                }
            }
            result->addAddressLabel(addresslabel);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "organization");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); ) {
            VCard::Organization organization;
            organization.name = Lua::getStringField(L, -1, "name").get_value_or("");
            lua_getfield(L, -1, "units");
            if (lua_istable(L, -1)) {
                for (lua_pushnil(L); lua_next(L, -2); ) {
                    if (lua_isstring(L, -1)) {
                        organization.units.push_back(lua_tostring(L, -1));
                    }
                        lua_pop(L, 1);
                    }
            }
            lua_pop(L, 1);
            result->addOrganization(organization);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "jid");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); ) {
            if (lua_isstring(L, -1)) {
                result->addJID(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "title");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); ) {
            if (lua_isstring(L, -1)) {
                result->addTitle(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "role");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); ) {
            if (lua_isstring(L, -1)) {
                result->addRole(lua_tostring(L, -1));
            }
            lua_pop(L, 1);        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "url");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); ) {
            if (lua_isstring(L, -1)) {
                result->addURL(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    return result;
}

void VCardConvertor::doConvertToLua(lua_State* L, std::shared_ptr<VCard> payload) {
    lua_newtable(L);
    if (!payload->getFullName().empty()) {
        lua_pushstring(L, payload->getFullName().c_str());
        lua_setfield(L, -2, "fullname");
    }
    if (!payload->getFamilyName().empty()) {
        lua_pushstring(L, payload->getFamilyName().c_str());
        lua_setfield(L, -2, "familyname");
    }
    if (!payload->getGivenName().empty()) {
        lua_pushstring(L, payload->getGivenName().c_str());
        lua_setfield(L, -2, "givenname");
    }
    if (!payload->getMiddleName().empty()) {
        lua_pushstring(L, payload->getMiddleName().c_str());
        lua_setfield(L, -2, "middlename");
    }
    if (!payload->getPrefix().empty()) {
        lua_pushstring(L, payload->getPrefix().c_str());
        lua_setfield(L, -2, "prefix");
    }
    if (!payload->getSuffix().empty()) {
        lua_pushstring(L, payload->getSuffix().c_str());
        lua_setfield(L, -2, "suffix");
    }
    if (!payload->getNickname().empty()) {
        lua_pushstring(L, payload->getNickname().c_str());
        lua_setfield(L, -2, "nick");
    }
    if (!payload->getDescription().empty()) {
        lua_pushstring(L, payload->getDescription().c_str());
        lua_setfield(L, -2, "description");
    }
    if (!payload->getPhoto().empty()) {
        lua_pushlstring(L, reinterpret_cast<const char*>(vecptr(payload->getPhoto())), payload->getPhoto().size());
        lua_setfield(L, -2, "photo");
    }
    if (!payload->getPhotoType().empty()) {
        lua_pushstring(L, payload->getPhotoType().c_str());
        lua_setfield(L, -2, "phototype");
    }
    if (!payload->getBirthday().is_not_a_date_time()) {
        lua_pushstring(L, dateTimeToString(payload->getBirthday()).c_str());
        lua_setfield(L, -2, "birthday");
    }
    const std::vector<VCard::EMailAddress>& emails = payload->getEMailAddresses();
    if (!emails.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(emails.size()), 0);
        for (size_t i = 0; i < emails.size(); ++i) {
            lua_createtable(L, 0, 0);
            if (!emails[i].address.empty()) {
                lua_pushstring(L, emails[i].address.c_str());
                lua_setfield(L, -2, "address");
            }
            if (emails[i].isHome) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "home");
            }
            if (emails[i].isWork) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "work");
            }
            if (emails[i].isInternet) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "internet");
            }
            if (emails[i].isPreferred) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "preferred");
            }
            if (emails[i].isX400) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "x400");
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "email");
    }
    const std::vector<VCard::Telephone>& telephones = payload->getTelephones();
    if (!telephones.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(telephones.size()), 0);
        for (size_t i = 0; i < telephones.size(); ++i) {
            lua_createtable(L, 0, 0);
            if (!telephones[i].number.empty()) {
                lua_pushstring(L, telephones[i].number.c_str());
                lua_setfield(L, -2, "number");
            }
            if (telephones[i].isHome) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "home");
            }
            if (telephones[i].isWork) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "work");
            }
            if (telephones[i].isVoice) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "voice");
            }
            if (telephones[i].isFax) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "fax");
            }
            if (telephones[i].isPager) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "pager");
            }
            if (telephones[i].isMSG) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "msg");
            }
            if (telephones[i].isCell) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "cell");
            }
            if (telephones[i].isVideo) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "video");
            }
            if (telephones[i].isBBS) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "bbs");
            }
            if (telephones[i].isModem) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "modem");
            }
            if (telephones[i].isISDN) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "isdn");
            }
            if (telephones[i].isPCS) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "pcs");
            }
            if (telephones[i].isPreferred) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "preferred");
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "telephone");
    }
    const std::vector<VCard::Address>& addresses = payload->getAddresses();
    if (!addresses.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(addresses.size()), 0);
        for (size_t i = 0; i < addresses.size(); ++i) {
            lua_createtable(L, 0, 0);
            if (!addresses[i].poBox.empty()) {
                lua_pushstring(L, addresses[i].poBox.c_str());
                lua_setfield(L, -2, "pobox");
            }
            if (!addresses[i].addressExtension.empty()) {
                lua_pushstring(L, addresses[i].addressExtension.c_str());
                lua_setfield(L, -2, "extension");
            }
            if (!addresses[i].street.empty()) {
                lua_pushstring(L, addresses[i].street.c_str());
                lua_setfield(L, -2, "street");
            }
            if (!addresses[i].locality.empty()) {
                lua_pushstring(L, addresses[i].locality.c_str());
                lua_setfield(L, -2, "locality");
            }
            if (!addresses[i].region.empty()) {
                lua_pushstring(L, addresses[i].region.c_str());
                lua_setfield(L, -2, "region");
            }
            if (!addresses[i].postalCode.empty()) {
                lua_pushstring(L, addresses[i].postalCode.c_str());
                lua_setfield(L, -2, "postalcode");
            }
            if (!addresses[i].country.empty()) {
                lua_pushstring(L, addresses[i].country.c_str());
                lua_setfield(L, -2, "country");
            }
            if (addresses[i].isHome) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "home");
            }
            if (addresses[i].isWork) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "work");
            }
            if (addresses[i].isPostal) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "postal");
            }
            if (addresses[i].isParcel) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "parcel");
            }
            if (addresses[i].isPreferred) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "preferred");
            }
            if (addresses[i].deliveryType == VCard::DomesticDelivery) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "domestic");
            }
            if (addresses[i].deliveryType == VCard::InternationalDelivery) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "international");
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "address");
    }
    const std::vector<VCard::AddressLabel>& addresslabels = payload->getAddressLabels();
    if (!addresslabels.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(addresslabels.size()), 0);
        for (size_t i = 0; i < addresslabels.size(); ++i) {
            lua_createtable(L, 0, 0);
            const std::vector<std::string>& lines = addresslabels[i].lines;
            if (!lines.empty()) {
                lua_createtable(L, boost::numeric_cast<int>(addresslabels[i].lines.size()), 0);
                for (size_t j = 0; j < lines.size(); ++j) {
                    if (!lines[j].empty()) {
                        lua_pushstring(L, lines[j].c_str());
                    }
                    lua_rawseti(L, -2, boost::numeric_cast<int>(j+1));
                }
                lua_setfield(L, -2, "lines");
            }
            if (addresslabels[i].isHome) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "home");
            }
            if (addresslabels[i].isWork) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "work");
            }
            if (addresslabels[i].isPostal) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "postal");
            }
            if (addresslabels[i].isParcel) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "parcel");
            }
            if (addresslabels[i].isPreferred) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "preferred");
            }
            if (addresslabels[i].deliveryType == VCard::DomesticDelivery) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "domestic");
            }
            if (addresslabels[i].deliveryType == VCard::InternationalDelivery) {
                lua_pushboolean(L, true);
                lua_setfield(L, -2, "international");
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "addresslabel");
    }
    const std::vector<VCard::Organization>& organizations = payload->getOrganizations();
    if (!organizations.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(organizations.size()), 0);
        for (size_t i = 0; i < organizations.size(); ++i) {
            lua_createtable(L, 0, 0);
            if (!organizations[i].name.empty()) {
                lua_pushstring(L, organizations[i].name.c_str());
                lua_setfield(L, -2, "name");
            }
            const std::vector<std::string>& units = organizations[i].units;
            if (!units.empty()) {
                lua_createtable(L, boost::numeric_cast<int>(organizations[i].units.size()), 0);
                for (size_t j = 0; j < units.size(); ++j) {
                    if (!units[j].empty()) {
                        lua_pushstring(L, units[j].c_str());
                    }
                    lua_rawseti(L, -2, boost::numeric_cast<int>(j+1));
                }
                lua_setfield(L, -2, "units");
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "organization");
    }
    const std::vector<JID>& jids = payload->getJIDs();
    if (!jids.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(jids.size()), 0);
        for (size_t i = 0; i < jids.size(); ++i) {
            if (!jids[i].toString().empty()) {
                lua_pushstring(L, jids[i].toString().c_str());
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "jid");
    }
    const std::vector<std::string>& titles = payload->getTitles();
    if (!titles.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(titles.size()), 0);
        for (size_t i = 0; i < titles.size(); ++i) {
            if (!titles[i].empty()) {
                lua_pushstring(L, titles[i].c_str());
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "title");
    }
    const std::vector<std::string>& roles = payload->getRoles();
    if (!roles.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(roles.size()), 0);
        for (size_t i = 0; i < roles.size(); ++i) {
            if (!roles[i].empty()) {
                lua_pushstring(L, roles[i].c_str());
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "role");
    }
    const std::vector<std::string>& urls = payload->getURLs();
    if (!urls.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(urls.size()), 0);
        for (size_t i = 0; i < urls.size(); ++i) {
            if (!urls[i].empty()) {
                lua_pushstring(L, urls[i].c_str());
            }
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        lua_setfield(L, -2, "url");
    }
}
