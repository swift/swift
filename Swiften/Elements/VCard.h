/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/date_time/posix_time/ptime.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API VCard : public Payload {
        public:
            typedef std::shared_ptr<VCard> ref;

            struct EMailAddress {
                EMailAddress() : isHome(false), isWork(false), isInternet(false), isPreferred(false), isX400(false) {
                }

                bool isHome;
                bool isWork;
                bool isInternet;
                bool isPreferred;
                bool isX400;
                std::string address;
            };

            struct Telephone {
                Telephone() : isHome(false), isWork(false), isVoice(false), isFax(false), isPager(false), isMSG(false), isCell(false),
                    isVideo(false), isBBS(false), isModem(false), isISDN(false), isPCS(false), isPreferred(false) {
                }

                bool isHome;
                bool isWork;
                bool isVoice;
                bool isFax;
                bool isPager;
                bool isMSG;
                bool isCell;
                bool isVideo;
                bool isBBS;
                bool isModem;
                bool isISDN;
                bool isPCS;
                bool isPreferred;
                std::string number;
            };

            enum DeliveryType {
                DomesticDelivery,
                InternationalDelivery,
                None
            };

            struct Address {
                Address() : isHome(false), isWork(false), isPostal(false), isParcel(false), deliveryType(None), isPreferred(false) {
                }

                bool isHome;
                bool isWork;
                bool isPostal;
                bool isParcel;
                DeliveryType deliveryType;
                bool isPreferred;

                std::string poBox;
                std::string addressExtension;
                std::string street;
                std::string locality;
                std::string region;
                std::string postalCode;
                std::string country;
            };

            struct AddressLabel {
                AddressLabel() : isHome(false), isWork(false), isPostal(false), isParcel(false), deliveryType(None), isPreferred(false) {
                }

                bool isHome;
                bool isWork;
                bool isPostal;
                bool isParcel;
                DeliveryType deliveryType;
                bool isPreferred;
                std::vector<std::string> lines;
            };

            struct Organization {
                std::string name;
                std::vector<std::string> units;
            };

            VCard() {}

            void setVersion(const std::string& version) { version_ = version; }
            const std::string& getVersion() const { return version_; }

            void setFullName(const std::string& fullName) { fullName_ = fullName; }
            const std::string& getFullName() const { return fullName_; }

            void setFamilyName(const std::string& familyName) { familyName_ = familyName; }
            const std::string& getFamilyName() const { return familyName_; }

            void setGivenName(const std::string& givenName) { givenName_ = givenName; }
            const std::string& getGivenName() const { return givenName_; }

            void setMiddleName(const std::string& middleName) { middleName_ = middleName; }
            const std::string& getMiddleName() const { return middleName_; }

            void setPrefix(const std::string& prefix) { prefix_ = prefix; }
            const std::string& getPrefix() const { return prefix_; }

            void setSuffix(const std::string& suffix) { suffix_ = suffix; }
            const std::string& getSuffix() const { return suffix_; }


            //void setEMailAddress(const std::string& email) { email_ = email; }
            //const std::string& getEMailAddress() const { return email_; }

            void setNickname(const std::string& nick) { nick_ = nick; }
            const std::string& getNickname() const { return nick_; }

            void setPhoto(const ByteArray& photo) { photo_ = photo; }
            const ByteArray& getPhoto() const { return photo_; }

            void setPhotoType(const std::string& photoType) { photoType_ = photoType; }
            const std::string& getPhotoType() const { return photoType_; }

            const std::string& getUnknownContent() const { return unknownContent_; }
            void addUnknownContent(const std::string& c) {
                unknownContent_ += c;
            }

            const std::vector<EMailAddress>& getEMailAddresses() const {
                return emailAddresses_;
            }

            void addEMailAddress(const EMailAddress& email) {
                emailAddresses_.push_back(email);
            }

            void clearEMailAddresses() {
                emailAddresses_.clear();
            }

            EMailAddress getPreferredEMailAddress() const;

            void setBirthday(const boost::posix_time::ptime& birthday) {
                birthday_ = birthday;
            }

            const boost::posix_time::ptime& getBirthday() const {
                return birthday_;
            }

            const std::vector<Telephone>& getTelephones() const {
                return telephones_;
            }

            void addTelephone(const Telephone& phone) {
                telephones_.push_back(phone);
            }

            void clearTelephones() {
                telephones_.clear();
            }

            const std::vector<Address>& getAddresses() const {
                return addresses_;
            }

            void addAddress(const Address& address) {
                addresses_.push_back(address);
            }

            void clearAddresses() {
                addresses_.clear();
            }

            const std::vector<AddressLabel>& getAddressLabels() const {
                return addressLabels_;
            }

            void addAddressLabel(const AddressLabel& addressLabel) {
                addressLabels_.push_back(addressLabel);
            }

            void clearAddressLabels() {
                addressLabels_.clear();
            }

            const std::vector<JID>& getJIDs() const {
                return jids_;
            }

            void addJID(const JID& jid) {
                jids_.push_back(jid);
            }

            void clearJIDs() {
                jids_.clear();
            }

            const std::string& getDescription() const {
                return description_;
            }

            void setDescription(const std::string& description) {
                this->description_ = description;
            }

            const std::vector<Organization>& getOrganizations() const {
                return organizations_;
            }

            void addOrganization(const Organization& organization) {
                organizations_.push_back(organization);
            }

            void clearOrganizations() {
                organizations_.clear();
            }

            const std::vector<std::string>& getTitles() const {
                return titles_;
            }

            void addTitle(const std::string& title) {
                titles_.push_back(title);
            }

            void clearTitles() {
                titles_.clear();
            }

            const std::vector<std::string>& getRoles() const {
                return roles_;
            }

            void addRole(const std::string& role) {
                roles_.push_back(role);
            }

            void clearRoles() {
                roles_.clear();
            }

            const std::vector<std::string>& getURLs() const {
                return urls_;
            }

            void addURL(const std::string& url) {
                urls_.push_back(url);
            }

            void clearURLs() {
                urls_.clear();
            }

            bool isEmpty() const {
                bool empty = version_.empty() && fullName_.empty() && familyName_.empty() && givenName_.empty() && middleName_.empty() && prefix_.empty() && suffix_.empty();
                empty &= photo_.empty() && photoType_.empty() && nick_.empty();
                empty &= birthday_.is_not_a_date_time();
                empty &= unknownContent_.empty();
                empty &= emailAddresses_.empty() && telephones_.empty() && addresses_.empty() && addressLabels_.empty() && jids_.empty();
                empty &= description_.empty() && organizations_.empty() && titles_.empty() && roles_.empty() && urls_.empty();
                return empty;
            }

        private:
            std::string version_;
            std::string fullName_;
            std::string familyName_;
            std::string givenName_;
            std::string middleName_;
            std::string prefix_;
            std::string suffix_;
            //std::string email_;
            ByteArray photo_;
            std::string photoType_;
            std::string nick_;
            boost::posix_time::ptime birthday_;
            std::string unknownContent_;
            std::vector<EMailAddress> emailAddresses_;
            std::vector<Telephone> telephones_;
            std::vector<Address> addresses_;
            std::vector<AddressLabel> addressLabels_;
            std::vector<JID> jids_;
            std::string description_;
            std::vector<Organization> organizations_;
            std::vector<std::string> titles_;
            std::vector<std::string> roles_;
            std::vector<std::string> urls_;
    };
}
