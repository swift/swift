/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Base/Shared.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class VCard : public Payload, public Shared<VCard> {
		public:
			struct EMailAddress {
				EMailAddress() : isHome(false), isWork(false), isInternet(false), isPreferred(false), isX400(false) {
				}

				bool isHome;
				bool isWork;
				bool isInternet;
				bool isPreferred;
				bool isX400;
				String address;
			};

			VCard() {}

			void setVersion(const String& version) { version_ = version; }
			const String& getVersion() const { return version_; }

			void setFullName(const String& fullName) { fullName_ = fullName; }
			const String& getFullName() const { return fullName_; }

			void setFamilyName(const String& familyName) { familyName_ = familyName; }
			const String& getFamilyName() const { return familyName_; }

			void setGivenName(const String& givenName) { givenName_ = givenName; }
			const String& getGivenName() const { return givenName_; }

			void setMiddleName(const String& middleName) { middleName_ = middleName; }
			const String& getMiddleName() const { return middleName_; }

			void setPrefix(const String& prefix) { prefix_ = prefix; }
			const String& getPrefix() const { return prefix_; }

			void setSuffix(const String& suffix) { suffix_ = suffix; }
			const String& getSuffix() const { return suffix_; }


			//void setEMailAddress(const String& email) { email_ = email; }
			//const String& getEMailAddress() const { return email_; }

			void setNickname(const String& nick) { nick_ = nick; }
			const String& getNickname() const { return nick_; }

			void setPhoto(const ByteArray& photo) { photo_ = photo; }
			const ByteArray& getPhoto() { return photo_; }

			void setPhotoType(const String& photoType) { photoType_ = photoType; }
			const String& getPhotoType() { return photoType_; }

			const String& getUnknownContent() const { return unknownContent_; }
			void addUnknownContent(const String& c) { 
				unknownContent_ += c;
			}

			const std::vector<EMailAddress> getEMailAddresses() const {
				return emailAddresses_;
			}

			void addEMailAddress(const EMailAddress& email) {
				emailAddresses_.push_back(email);
			}

			EMailAddress getPreferredEMailAddress() const;

		private:
			String version_;
			String fullName_;
			String familyName_;
			String givenName_;
			String middleName_;
			String prefix_;
			String suffix_;
			//String email_;
			ByteArray photo_;
			String photoType_;
			String nick_;
			String unknownContent_;
			std::vector<EMailAddress> emailAddresses_;
	};
}
