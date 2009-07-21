#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class VCard : public Payload {
		public:
			VCard() {}

			void setFullName(const String& fullName) { fullName_ = fullName; }
			const String& getFullName() const { return fullName_; }

			void setFamilyName(const String& familyName) { familyName_ = familyName; }
			const String& getFamilyName() const { return familyName_; }

			void setGivenName(const String& givenName) { givenName_ = givenName; }
			const String& getGivenName() const { return givenName_; }

			void setEMail(const String& email) { email_ = email; }
			const String& getEMail() const { return email_; }

			void setNickname(const String& nick) { nick_ = nick; }
			const String& getNickname() const { return nick_; }

			void setPhoto(const ByteArray& photo) { photo_ = photo; }
			const ByteArray& getPhoto() { return photo_; }

			void setPhotoType(const String& photoType) { photoType_ = photoType; }
			const String& getPhotoType() { return photoType_; }

		private:
			String fullName_;
			String familyName_;
			String givenName_;
			String email_;
			ByteArray photo_;
			String photoType_;
			String nick_;
	};
}
