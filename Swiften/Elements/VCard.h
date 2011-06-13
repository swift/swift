/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class VCard : public Payload {
		public:
			typedef boost::shared_ptr<VCard> ref;

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

			EMailAddress getPreferredEMailAddress() const;

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
			std::string unknownContent_;
			std::vector<EMailAddress> emailAddresses_;
	};
}
